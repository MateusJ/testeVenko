#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>

using namespace std;
int main() {
    //Variaveis
    const int port = 8080;
    char buffer[1024];

    // Criando server
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        cerr << "Erro na criação do servidor" << endl;
        return -1;
    }

    // Configuração do servidor
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);  
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Vinculação porta e socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        cerr << "Erro na vinculação do socket." << endl;
        close(serverSocket);
        return -1;
    }

    // Servidor em modo Listen
    if (listen(serverSocket, 10) == -1) {
        cerr << "Erro ao colocar o servidor em modo listen." << endl;
        close(serverSocket);
        return -1;
    }

    cout << "Espernado..." << endl;

    // Aceitando a Conexeção
    sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);

    if (clientSocket == -1) {
        std::cerr << "Erro ao aceitar a conexão." << std::endl;
        close(serverSocket);
        return -1;
    }

    cout << "Cliente conectado." << endl;

    // Lê dados do cliente
    
    int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead == -1) {
        std::cerr << "Erro ao receber dados do cliente." << std::endl;
    } else {
        std::cout << "Mensagem do cliente: " << buffer << std::endl;
    }

    // Fecha os sockets
    close(clientSocket);
    close(serverSocket);

    return 0;
}