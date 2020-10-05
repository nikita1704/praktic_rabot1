// MsgClient.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include "framework.h"
#include "MsgClient.h"
#include "Msg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



void ProcessClient(SOCKET hSock, Message& Msg, int Id)
{
    CSocket s;
    s.Attach(hSock);
    for(;;) {
        cout << "Отправить сообщение - 0; Выйти -1: ";
        bool Prov;
        cin >> Prov;
        switch (Prov)
        {
        case 0:
        {
            bool MsgType;
            string Data;
            cout << "Личное - 0; Широковещательное - 1: ";
            cin >> MsgType;
            cout << "Введите сообщение: ";
            cin >> Data;
            switch (MsgType)
            {
            case 0:
            {
                int IdRec;
                cout << "Введите получателя: ";
                cin >> IdRec;
                Message::Send(s, IdRec, Id, M_DATA, Data);
                break;
            }
            case 1:
            {
                Message::Send(s, M_ALL, Id, M_DATA, Data);
                break;
            }
            }
            break;
        }
        case 1:
        {
            Message::Send(s, M_BROKER, Id, M_EXIT);
            return;
        }
        }
    }
}

void ReceiveMsg(SOCKET hSock, Message Msg, int Id)
{
    CSocket s;
    s.Attach(hSock);
    for(;;) {
        Sleep(1000);
        Message::Send(s, M_BROKER, Id, M_GETDATA);
        if (Msg.Receive(s) == M_DATA)
            cout << endl << "Получено сообщение от " << Msg.m_Header.m_From << ": " << Msg.m_Data;
       
    }
}


void Client()
{
    setlocale(LC_ALL, "Russian");
    AfxSocketInit();
    CSocket Socket;
    Socket.Create();
    Socket.Connect("127.0.0.1", 12345);
    Message Msg;
    int Id;
    Message::Send(Socket, M_BROKER, 0, M_INIT);
    if (Msg.Receive(Socket) == M_INIT)
        Id = Msg.m_Header.m_To;
    auto S = Socket.Detach();
    thread t(ProcessClient, S, Msg, ref(Id));
    t.detach();
    thread tRM(ReceiveMsg, S, Msg, ref(Id));
    tRM.join();
}


// Единственный объект приложения

CWinApp theApp;

using namespace std;

int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // инициализировать MFC, а также печать и сообщения об ошибках про сбое
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: вставьте сюда код для приложения.
            wprintf(L"Критическая ошибка: сбой при инициализации MFC\n");
            nRetCode = 1;
        }
        else
        {
            // TODO: вставьте сюда код для приложения.
            Client();
        }
    }
    else
    {
        // TODO: измените код ошибки в соответствии с потребностями
        wprintf(L"Критическая ошибка: сбой GetModuleHandle\n");
        nRetCode = 1;
    }

    return nRetCode;
}
