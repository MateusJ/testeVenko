#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <dirent.h>
#include <fstream>
#include <cstdio>

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

void enviarArquivo(int clientSocket, const char* arquivoEscolhido){

    string novoArquivo = "./arquivos/" + string(arquivoEscolhido);
    cout << novoArquivo << endl;

    ifstream arquivo(novoArquivo, ios::binary | ios::ate);

    if(!arquivo.is_open()){
        cerr << "Erro ao abrir arquivo" << endl;
        return;
    }

    streamsize tamanhoArquivo = arquivo.tellg();
    arquivo.seekg(0,ios::beg);

    send(clientSocket, &tamanhoArquivo, sizeof(tamanhoArquivo), 0);

    char buffer[1024];
    while(!arquivo.eof()){
        arquivo.read(buffer, sizeof(buffer));
        send(clientSocket,buffer,arquivo.gcount(),0);
    }
    arquivo.close();

}

void deleteArquivo(const char* arquivoEscolhido){

    cout << arquivoEscolhido << endl;

    string deletarArquivo = "./arquivos/" + string(arquivoEscolhido);

    cout << deletarArquivo << endl;

    if(remove(deletarArquivo.c_str()) != 0){
        cerr << "Problemas ao remover arquivo" << endl;
    }else{
        cout << "Removido com sucesso" << endl;
    }

}

void receberArquivo(int clientSocket){

    string novoArquivo = receberMsg(clientSocket);
    cout <<  novoArquivo << endl;
    enviarMsg(clientSocket, "OK");
    
    ofstream arquivo(novoArquivo, ios::binary);

    if(!arquivo.is_open()){
        cerr << "Erro ao criar arquivo" << endl;
        return;
    }

    

    streamsize tamanhoArquivo;
    recv(clientSocket, &tamanhoArquivo, sizeof(tamanhoArquivo),0);
    cout << "tamanho arquivo" << tamanhoArquivo << endl;
    enviarMsg(clientSocket, "OK");

    char buffer[1024];
    streamsize bytes = 0;
    while(bytes < tamanhoArquivo){
        int recebido = recv(clientSocket, buffer, sizeof(buffer),0);
        if(recebido == -1){
            cerr << "Erro ao receber arquivo" << endl;
            break;
        }

        arquivo.write(buffer,recebido);
        bytes += recebido;
    }
    arquivo.close();
    
}

int main() {
    //Variaveis
    const int port = 8080;
    char buffer[1024];
    int opcaoEscolhida = 0;
    string todosArquivos = "", arquivoEscolhido = "";

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

        while(true){

            receberMsg(clientSocket);

            const char* resposta = "Opçõs:\n1. Listagem de arquivos\n2. Download de arquivo\n3. Deletar arquivo\n4. Upload de arquivos\n";
            send(clientSocket, resposta, strlen(resposta),0);
            

            opcaoEscolhida = stoi(receberMsg(clientSocket));

            switch(opcaoEscolhida){
                case 1:
                todosArquivos = listarArquivos();
                enviarMsg(clientSocket, todosArquivos.c_str());
                break;
                case 2:
                enviarMsg(clientSocket, "Deseja fazer download de qual arquivo?");
                arquivoEscolhido = receberMsg(clientSocket);
                cout <<  arquivoEscolhido<< endl;
                enviarArquivo(clientSocket, arquivoEscolhido.c_str());
                
                break;
                case 3:
                enviarMsg(clientSocket, "Qual arquivo deseja deletar?");
                arquivoEscolhido = receberMsg(clientSocket);
                cout << arquivoEscolhido << endl;
                deleteArquivo(arquivoEscolhido.c_str());
                break;
                case 4:
                enviarMsg(clientSocket, "Escolha um arquivo para fazer upload:");
                receberArquivo(clientSocket);
                break;
                default:
                goto endwhile;
            }

        }
        endwhile:;

        
    // Fecha os sockets
    close(clientSocket);
    close(serverSocket);

    return 0;
}