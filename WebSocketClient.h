#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H_

#include <string.h>
#include <stdlib.h>
#include <WString.h>
#include <Ethernet.h>
#include "Arduino.h"

class WebSocketClient {
	public:
        WebSocketClient(byte server[], String path, int port = 80, int timeout = 3000);
		typedef void (*DataArrivedDelegate)(WebSocketClient client, String data);
		bool connect();
        bool connected();
        void disconnect();
		void monitor();
		void setDataArrivedDelegate(DataArrivedDelegate dataArrivedDelegate);
		void send(String data);
        void sendHandshake();
	private:
		EthernetClient _client;
        int    _port;
        String _path;
        String _hostname;
		byte*  _server;
		int    _timeout;
        DataArrivedDelegate _dataArrivedDelegate;
        bool readHandshake();
        String readLine();
};


#endif
