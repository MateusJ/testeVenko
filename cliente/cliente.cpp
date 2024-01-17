#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>

using namespace std;

int main() {
    //Variaveis
    const int port = 8080;
    const string ip = "127.0.0.1";

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
    const char* message = "Hello Server!";
    if (send(clientSocket, message, strlen(message), 0) == -1) {
        cerr << "Erro ao enviar dados para o servidor." << endl;
    } else {
        cout << "Dados enviados para o servidor." << endl;
    }

    // Fecha o socket do cliente
    close(clientSocket);

    return 0;
}
