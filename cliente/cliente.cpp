#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <fstream>

using namespace std;

string receberMsg(int clientSocket) {
    char buffer[1024];
    int bytesRecebidos = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRecebidos == -1) {
        cerr << "Erro ao receber dados" << endl;
    } else {
        buffer[bytesRecebidos] = '\0';
        return string(buffer);
    }
}

void enviarMsg(int clientSocket, const char* mensagem) {
    if (send(clientSocket, mensagem, strlen(mensagem), 0) == -1) {
        cerr << "Erro ao enviar dados para o servidor." << endl;
    } else {
        cout << "Dados enviados para o servidor." << endl;
    }
}

void receberArquivo(int clientSocket, const char* novoArquivo){

    ofstream arquivo(novoArquivo, ios::binary);

    if(!arquivo.is_open()){
        cerr << "Erro ao criar arquivo" << endl;
        return;
    }

    streamsize tamanhoArquivo;
    recv(clientSocket, &tamanhoArquivo, sizeof(tamanhoArquivo), 0);

    char buffer[1024];
    streamsize bytes = 0;
    while(bytes < tamanhoArquivo){
        int recebido = recv(clientSocket, buffer, sizeof(buffer),0);
        if(recebido == -1){
            cerr << "Erro ao receber arquivo" << endl;
            break;
        }

        arquivo.write(buffer, recebido);
        bytes += recebido;
    }
    arquivo.close();

}

void enviarArquivo(int clientSocket, const char* arquivoEscolhido){

    string uploadArquivo = "./uploads/" + string(arquivoEscolhido);
    
    ifstream arquivo(uploadArquivo, ios::binary | ios::ate);

    if(!arquivo.is_open()){
        cerr << "Erro ao abrir arquivo" << endl;
        return;
    }

    enviarMsg(clientSocket, arquivoEscolhido);
    receberMsg(clientSocket);
    
    streamsize tamanhoArquivo = arquivo.tellg();
    arquivo.seekg(0,ios::beg);
    

    send(clientSocket, &tamanhoArquivo, sizeof(tamanhoArquivo),0);
    receberMsg(clientSocket);

    char buffer[1024];
    while(!arquivo.eof()){
        arquivo.read(buffer, sizeof(buffer));
        send(clientSocket,buffer,arquivo.gcount(),0);
    }
    arquivo.close();

}

int main() {
    const int port = 8080;
    const string ip = "127.0.0.1";
    int escolhaOpcao = 0;
    string arquivoEscolhido = "", escolherAuxiliar = "";

    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        cerr << "Erro ao criar o socket do cliente." << endl;
        return -1;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        cerr << "Erro ao conectar ao servidor." << endl;
        close(clientSocket);
        return -1;
    }

    enviarMsg(clientSocket, "Hello teste!");

    while (true) {

        enviarMsg(clientSocket, "ok");

        cout << receberMsg(clientSocket) << endl;
        cout << "Escolha uma opção: ";
        cin >> escolhaOpcao;

        string opcaoEscolhida = to_string(escolhaOpcao);
        enviarMsg(clientSocket, opcaoEscolhida.c_str());

        switch (escolhaOpcao) {
            case 1:
                cout << receberMsg(clientSocket) << endl;
                break;
            case 2:
                cout << receberMsg(clientSocket) << endl;
                cout << "Escolha um arquivo (número): ";
                cin >> escolherAuxiliar;
                enviarMsg(clientSocket, escolherAuxiliar.c_str());

                receberArquivo(clientSocket, "novoArquivo");
                break;
            case 3:
                cout << receberMsg(clientSocket) << endl;
                cin >> escolherAuxiliar;
                enviarMsg(clientSocket, escolherAuxiliar.c_str());
            break;
            case 4:
                cout << receberMsg(clientSocket) << endl;
                cin >> escolherAuxiliar;
                enviarArquivo(clientSocket, escolherAuxiliar.c_str());
            break;
            default:
                close(clientSocket);
                return 0;
        }
    }

    // O código aqui não será alcançado, mas para garantir uma boa prática
    close(clientSocket);
    return 0;
}
