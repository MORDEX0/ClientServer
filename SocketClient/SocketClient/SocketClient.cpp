#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>

using namespace std;

int main() {
    WSADATA wsaData; //объявление переменной, которая используется для хранения информации о инициализации библиотеки сокетов Windows
    ADDRINFO hints; //объявление переменной, которая используется для указания предпочтительных параметров при вызове функции getaddrinfo
    ADDRINFO* addrResult; //указатель, который будет использоваться для хранения результатов функции getaddrinfo
    SOCKET ConnectSocket = INVALID_SOCKET; //эта переменная используется для установки соединения с клиентом
    char recvBuffer[512]; //объявляет массив символов, который будет использоваться для приема данных от клиента

    const char* sendBuffer1 = "Hello from client 1"; //строка, которая содержит сообщение, которое будет сохранено в буфер
    const char* sendBuffer2 = "Hello from client 2"; //вторая строка, которая содержит сообщение, которое будет сохранено в буфер

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData); //объявление переменной, которая используется для хранения информации о инициализации библиотеки сокетов Windows
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

    result = getaddrinfo("localhost", "666", &hints, &addrResult); //  Эта строка вызывает функцию getaddrinfo для получения информации об адресе сокета, который будет слушать входящие соединения
    //В этом блоке проверяется результат вызова функции getaddrinfo. 
    // Если получение информации об адресе не удалось, выводится сообщение об ошибке
    if (result != 0) {
        cout << "getaddrinfo failed with error: " << result << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);//Создается сокет для соединения с использованием параметров, полученных из addrResult.
    //Проверяется, был ли успешно создан сокет. Если нет, выводится сообщение об ошибке 
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Socket creation failed" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);//Устанавливается соединение с сервером с использованием созданного сокета и адреса, полученного из addrResult.
    //Проверяется, было ли успешно установлено соединение. Если нет, выводится сообщение об ошибке
    if (result == SOCKET_ERROR) {
        cout << "Unable to connect to server" << endl;
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }

    result = send(ConnectSocket, sendBuffer1, (int)strlen(sendBuffer1), 0);//Отправляются данные на сервер через установленное соединение.
    //Проверяется, была ли успешно отправлена информация. Если нет, выводится сообщение об ошибке
    if (result == SOCKET_ERROR) {
        cout << "Send failed, error: " << result << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    cout << "Sent: " << result << " bytes" << endl;

    result = send(ConnectSocket, sendBuffer2, (int)strlen(sendBuffer2), 0);//Отправляются данные на сервер через установленное соединение.
    //Проверяется, была ли успешно отправлена информация. Если нет, выводится сообщение об ошибке
    if (result == SOCKET_ERROR) {
        cout << "Send failed, error: " << result << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    cout << "Sent: " << result << " bytes" << endl;

    result = shutdown(ConnectSocket, SD_SEND); //Отправляется команда на завершение передачи данных по установленному соединению
    //Проверяется, была ли успешно отправлена команда на завершение передачи данных.
    //  Если нет, выводится сообщение об ошибке
    if (result == SOCKET_ERROR) {
        cout << "Shutdown failed, error: " << result << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    //Проверяется, была ли успешно отправлена команда на завершение передачи данных.
    //  Если нет, выводится сообщение об ошибке
    do {
        ZeroMemory(recvBuffer, 512);
        result = recv(ConnectSocket, recvBuffer, 512, 0);
        if (result > 0) {
            cout << "Received " << result << " bytes" << endl;
            cout << "Received data: " << recvBuffer << endl;
        }
        else if (result == 0) {
            cout << "Connection closed" << endl;
        }
        else {
            cout << "Recv failed, error: " << WSAGetLastError() << endl;
        }
    } while (result > 0);
    //Закрываются сокеты, освобождаются ресурсы и программа завершает свою работу успешно
    closesocket(ConnectSocket);
    freeaddrinfo(addrResult);
    WSACleanup();
    return 0;
}
