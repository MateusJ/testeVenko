#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>

using namespace std;

string receberMsg(int clientSocket){

    char buffer[1024];

    int bytesRead = recv(clientSocket,buffer,sizeof(buffer),0);
    if(bytesRead == -1){
        cerr << "Erro na espera" << endl;
    }else{
        buffer[bytesRead] = '\0';
        return string(buffer);
    }

}

int main() {
    //Variaveis
    const int port = 8080;
    char buffer[1024];
    int opcaoEscolhida = 0;

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

    cout << "Esperando..." << endl;

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
    
        cout << receberMsg(clientSocket) << endl;

        const char* resposta = "Opções:\n1. Listagem de arquivos\n2. Download de arquivo\n3. Deletar arquivo\n4. Upload de arquivos\n";
        send(clientSocket, resposta, strlen(resposta),0);

        opcaoEscolhida = stoi(receberMsg(clientSocket));

        switch(opcaoEscolhida){
            case 1:
            cout << "foi : 1" << endl;
            break;
            case 2:
            cout << "foi : 2" << endl;
            break;
            case 3:
            cout << "foi : 3" << endl;
            break;
            case 4:
            cout << "foi : 4" << endl;
            break;
            default:
            break;
        }


    // Fecha os sockets
    close(clientSocket);
    close(serverSocket);

    return 0;
}