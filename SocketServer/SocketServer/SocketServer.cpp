#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

int main() {
    WSADATA wsaData; //объявление переменной, которая используется для хранения информации о инициализации библиотеки сокетов Windows
    ADDRINFO hints;  //объявление переменной, которая используется для указания предпочтительных параметров при вызове функции getaddrinfo
    ADDRINFO* addrResult;  //указатель, который будет использоваться для хранения результатов функции getaddrinfo
    SOCKET ListenSocket = INVALID_SOCKET;  //эта переменная используется для прослушивания входящих соединений
    SOCKET ConnectSocket = INVALID_SOCKET;  //эта переменная используется для установки соединения с клиентом
    char recvBuffer[512];  //объявляет массив символов, который будет использоваться для приема данных от клиента

    const char* sendBuffer = "Hello from server"; //строка, которая содержит сообщение, которое будет отправлено клиенту.

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);  //объявление переменной, которая используется для хранения информации о инициализации библиотеки сокетов Windows
    
    //в этом блоке проверяется результат инициализации библиотеки сокетов Windows
    //если инициализация не удалась, выводится сообщение об ошибке, освобождаются ресурсы
    if (result != 0) {
        cout << "WSAStartup failed with result: " << result << endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints)); //эта строка заполняет память
    hints.ai_family = AF_INET; //в этом блоке устанавливаются значения полей структуры hints,
                               //которые указывают на предпочтительные параметры для функции getaddrinfo
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    
    result = getaddrinfo(NULL, "666", &hints, &addrResult);//  Эта строка вызывает функцию getaddrinfo для получения информации об адресе сокета, который будет слушать входящие соединения
    //В этом блоке проверяется результат вызова функции getaddrinfo. 
    // Если получение информации об адресе не удалось, выводится сообщение об ошибке
    
    if (result != 0) {
        cout << "getaddrinfo failed with error: " << result << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    ListenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol); // эта строка создает сокет для прослушивания 
    //входящих соединений на основе информации об адресе из переменной addrResult.
    //В этом блоке проверяется результат создания сокета. 
    // Если создание сокета не удалось, выводится сообщение об ошибке
    if (ListenSocket == INVALID_SOCKET) {
        cout << "Socket creation failed" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    result = bind(ListenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen); //эта строка связывает созданный сокет с локальным адресом из переменной addrResult
    // В этом блоке проверяется результат связывания сокета. 
    // Если связывание не удалось, выводится сообщение об ошибке
    
    if (result == SOCKET_ERROR) {
        cout << "Bind failed, error: " << result << endl;
        closesocket(ListenSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    result = listen(ListenSocket, SOMAXCONN); //эта строка переводит сокет в режим прослушивания входящих соединений
    //В этом блоке кода проверяется результат вызова функции listen.
    //Если перевод сокета в режим прослушивания не удался, выводится сообщение об ошибке
    
    if (result == SOCKET_ERROR) {
        cout << "Listen failed, error: " << result << endl;
        closesocket(ListenSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    ConnectSocket = accept(ListenSocket, NULL, NULL); //эта строка принимает входящее соединение на слушающем сокете и создает новый сокет для общения с клиентом.
    //В этом блоке кода проверяется результат вызова функции accept.
    //Если принятие соединения не удалось, выводится сообщение об ошибке
    
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Accept failed, error: " << WSAGetLastError() << endl;
        closesocket(ListenSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    closesocket(ListenSocket); //Эта строка закрывает слушающий сокет после установки соединения с клиентом
    //В этом блоке кода происходит циклическое чтение данных от клиента и отправка ответов до тех пор, 
    // пока соединение активно или происходят ошибки
    do {
        ZeroMemory(recvBuffer, 512);
        result = recv(ConnectSocket, recvBuffer, 512, 0);
        if (result > 0) {
            cout << "Received " << result << " bytes" << endl;
            cout << "Received data: " << recvBuffer << endl;

            result = send(ConnectSocket, sendBuffer, (int)strlen(sendBuffer), 0);
            if (result == SOCKET_ERROR) {
                cout << "Send failed, error: " << result << endl;
                closesocket(ConnectSocket);
                freeaddrinfo(addrResult);
                WSACleanup();
                return 1;
            }
        }
        else if (result == 0) {
            cout << "Connection closing" << endl;
        }
        else {
            cout << "Recv failed, error: " << WSAGetLastError() << endl;
            closesocket(ConnectSocket);
            freeaddrinfo(addrResult);
            WSACleanup();
            return 1;
        }
    } while (result > 0);

    result = shutdown(ConnectSocket, SD_SEND);//эта строка отправляет команду завершения передачи данных по сокету
    //в этом блоке кода проверяется результат отправки команды завершения передачи данных. 
    //если отправка не удалась, выводится сообщение об ошибке
    if (result == SOCKET_ERROR) {
        cout << "Shutdown failed, error: " << result << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    //закрытие сокетов, освобождение ресурсов и завершение работы программы
    closesocket(ConnectSocket);
    freeaddrinfo(addrResult);
    WSACleanup();
    return 0;
}
