#include <iostream>
#include <cstring> // manipulação de caracteres
#include <unistd.h> // Chamada de sistema (close)
#include <arpa/inet.h>
#include <string>
#include <dirent.h> //leitura de diretorios
#include <fstream>
#include <cstdio> //Varias funções, utilizado para remover arquivo.

using namespace std;

struct Pedido{
    char nome[50];
    int funcao;
    streamsize tamanho;
};

string receberMsg(int clientSocket){

    char buffer[1024];

    int bytesRead = recv(clientSocket,buffer,sizeof(buffer),0);
    if(bytesRead == -1){
        cerr << "Erro na espera" << endl;
    }else{
        buffer[bytesRead] = '\0'; //isso foi feito para evitar que caracteres indesejaveis apareçam
        return string(buffer);
    }

}

Pedido receberPedido(int clientSocket){

    Pedido pedido;

    int bytesRead = recv(clientSocket,&pedido,sizeof(Pedido),0);
    if(bytesRead == -1){
        cerr << "Erro na espera" << endl;
    }else{
        return pedido;
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
    }/**else {
        cout << "Dados enviados para o servidor." << endl;
    }**/

}

void enviarArquivo(int clientSocket, const char* arquivoEscolhido){

    Pedido download;

    string novoArquivo = "./arquivos/" + string(arquivoEscolhido);

    ifstream arquivo(novoArquivo, ios::binary | ios::ate);

    if(!arquivo.is_open()){
        cerr << "Erro ao abrir arquivo" << endl;
        return;
    }

    streamsize tamanhoArquivo = arquivo.tellg(); //Para verificar o tamanho do arquivo
    arquivo.seekg(0,ios::beg); // para pocisionar o ponteiro de leitura no inicio do arquivo

    strcpy(download.nome, arquivoEscolhido); //copiar o valor de uma variavel para outra.
    download.tamanho = tamanhoArquivo;

    send(clientSocket, &download, sizeof(Pedido),0);

    receberMsg(clientSocket);

    char bufferArquivo[1024];
    while(!arquivo.eof()){
        arquivo.read(bufferArquivo, sizeof(bufferArquivo));
        send(clientSocket,bufferArquivo,arquivo.gcount(),0);
    }
    arquivo.close();

}

void deleteArquivo(const char* arquivoEscolhido){


    string deletarArquivo = "./arquivos/" + string(arquivoEscolhido);


    if(remove(deletarArquivo.c_str()) != 0){
        cerr << "Problemas ao remover arquivo" << endl;
    }else{
        cout << "Removido com sucesso" << endl;
    }

}

void receberArquivo(int clientSocket){

    Pedido upload;

    recv(clientSocket, &upload, sizeof(Pedido), 0);
    enviarMsg(clientSocket, "OK");

    string caminho = "./arquivos/" + string(upload.nome);
    
    ofstream arquivo(caminho.c_str(), ios::binary);

    if(!arquivo.is_open()){
        cerr << "Erro ao criar arquivo" << endl;
        return;
    }

    char buffer[1024];
    streamsize bytes = 0;
    while(bytes < upload.tamanho){
        int recebido = recv(clientSocket, buffer, sizeof(buffer),0);
        if(recebido == -1){
            cerr << "Erro ao receber arquivo" << endl;
            break;
        }

        arquivo.write(buffer,recebido);
        bytes += recebido;
    }
    arquivo.close();
    enviarMsg(clientSocket, "OK");  
    
}

void escreverLog(string mensagem){

    ofstream log("log.txt", ios::app);

    if(!log.is_open()){
        cerr << "Erro ao criar arquivo" << endl;
        return;
    }

    int bytes;

    log << mensagem;

}

void checarLog(){

    FILE *log;
    if(log = fopen("log.txt", "r")){
        remove("log.txt");
    }

}

int main() {
    //Variaveis
    const int port = 8080;
    char buffer[1024];
    int opcaoEscolhida = 0;
    string todosArquivos = "", arquivoEscolhido = "";

    //Checa se o arquivo log ja existe e apaga se existir
    checarLog();

    // Criando server
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0); //SOCK_STREAM = TCP
    if (serverSocket == -1) {
        escreverLog("Erro ao criar servidor\n");
        return -1;
    }else{
        escreverLog("Servidor Criado com sucesso\n");
    }

    // Configuração do servidor
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);  
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Vinculação porta e socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        escreverLog("Erro na vinculação do Socket\n");
        close(serverSocket);
        return -1;
    }else{
        escreverLog("Socket vinculado com sucesso\n");
    }

    // Servidor em modo Listen
    if (listen(serverSocket, 10) == -1) {
        escreverLog("Erro ao colocar servidor em modo listen\n");
        close(serverSocket);
        return -1;
    }else{
        escreverLog("Servidor em modo listen\n");
    }

    cout << "Esperando..." << endl;

    // Aceitando a Conexeção
    sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);

    if (clientSocket == -1) {
        escreverLog("Erro na conexão/n");
        close(serverSocket);
        return -1;
    }

    escreverLog("Cliente Conectado\n");
    cout << "Cliente Conectado\n" << endl;


        while(true){

            Pedido pedido;

            //Checa se o cliente e o servidor estão na mesma etapa.
            receberMsg(clientSocket);
            enviarMsg(clientSocket, "OK");            

            //Recebe o pedido
            pedido = receberPedido(clientSocket);

            switch(pedido.funcao){
                case 1:
                    todosArquivos = listarArquivos();
                    enviarMsg(clientSocket, todosArquivos.c_str());
                break;
                case 2:
                    enviarArquivo(clientSocket, pedido.nome);
                break;
                case 3:
                    deleteArquivo(pedido.nome);
                break;
                case 4:
                    receberArquivo(clientSocket);
                break;
                default:
                    cout << "Até mais" << endl;
                goto endwhile;
            }

        }
        endwhile:;

        
    // Fecha os sockets
    close(clientSocket);
    close(serverSocket);

    return 0;
}