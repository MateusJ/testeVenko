#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <fstream>

using namespace std;

struct Arquivo{
    char nome[50];
    streamsize tamanho;
};

struct pedido{
    char nome[50];
    char funcao[15];
};

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

void receberArquivo(int clientSocket){

    Arquivo download;

    recv(clientSocket, &download, sizeof(Arquivo),0);
    cout << download.nome << endl;
    cout << download.tamanho << endl;
    enviarMsg(clientSocket, "OK");

    string caminho = "./downloads/" + string(download.nome);

    ofstream arquivo(caminho, ios::binary);

    if(!arquivo.is_open()){
        cerr << "Erro ao criar arquivo" << endl;
        return;
    }

    char buffer[1024];
    streamsize bytes = 0;
    while(bytes < download.tamanho){
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

        cout << "Opções:\n1. Listagem de arquivos\n2. Download de arquivo\n3. Deletar arquivo\n4. Upload de arquivos\n" << endl;
        cout << "Escolha uma opção: ";
        cin >> escolhaOpcao;

        string opcaoEscolhida = to_string(escolhaOpcao);
        enviarMsg(clientSocket, opcaoEscolhida.c_str());

        switch (escolhaOpcao) {
            case 1:
                cout << receberMsg(clientSocket) << endl;
                break;
            case 2:
                cout << "Deseja fazer download de qual arquivo? ";
                cin >> escolherAuxiliar;
                enviarMsg(clientSocket, escolherAuxiliar.c_str());

                receberArquivo(clientSocket);
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
