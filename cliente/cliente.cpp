#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>

using namespace std;

string receberMsg(int clientSocket){

    char buffer[1024];  
    int bytesRecebidos = recv(clientSocket, buffer, sizeof(buffer),0);
    if(bytesRecebidos == -1){
        cerr << "Erro ao receber dados" << endl;
    }else{
        buffer[bytesRecebidos] = '\0';
        return string(buffer);
    }
}

void enviarMsg(int clientSocket, const char* mensagem){

    if (send(clientSocket, mensagem, strlen(mensagem), 0) == -1) {
        cerr << "Erro ao enviar dados para o servidor." << endl;
    } else {
        cout << "Dados enviados para o servidor." << endl;
    }

}

int main() {
    //Variaveis
    const int port = 8080;
    const string ip = "127.0.0.1";
    int escolhaOpcao = 0;

    // Criação do socket do cliente
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        cerr << "Erro ao criar o socket do cliente." << endl;
        return -1;
    }

    // Configuração do endereço do servidor
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);  // Porta de comunicação
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str() );  // IP do servidor

    // Conexão ao servidor
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        cerr << "Erro ao conectar ao servidor." << endl;
        close(clientSocket);
        return -1;
    }

    // Envio de dados para o servidor
    enviarMsg(clientSocket, "Hello teste!");
    
    //receber mensagem de volta
    cout << receberMsg(clientSocket) << endl;

    //Escolher opção
    cin >> escolhaOpcao;

    string opcaoEscolhida = to_string(escolhaOpcao);

    enviarMsg(clientSocket,opcaoEscolhida.c_str());

    cout << receberMsg(clientSocket) << endl;
    
    // Fecha o socket do cliente
    close(clientSocket);

    return 0;
}
