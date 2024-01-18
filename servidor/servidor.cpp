#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <dirent.h>

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
string listarArquivos(){
    string todosArquivos;
    DIR* caminho = opendir("./arquivos");

    if(caminho){
        struct dirent* arquivo;
        while((arquivo = readdir(caminho))!= NULL){
            if(strcmp(arquivo->d_name, ".") != 0 && strcmp(arquivo->d_name, "..") != 0){
                todosArquivos = todosArquivos + "\n" + arquivo->d_name;
            }
        }
        closedir(caminho);
        return todosArquivos;
    }else{
        cout << "erro" << endl;
        return "0";
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
    char buffer[1024];
    int opcaoEscolhida = 0;
    string todosArquivos = "";

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
            todosArquivos = listarArquivos();
            enviarMsg(clientSocket, todosArquivos.c_str());
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