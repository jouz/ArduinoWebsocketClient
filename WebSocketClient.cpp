/*
 WebsocketClient, a websocket client for Arduino
 Copyright 2011 Kevin Rohling
 http://kevinrohling.com
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

#include <WebSocketClient.h>

WebSocketClient::WebSocketClient(byte server[], String path, int port, int timeout) {
    _port = port;
    _path = path;
	_server = server;
	_timeout = timeout;
    _hostname = String();
    int size = sizeof(server);
    for (int i = 0; i < size; i++) {
        _hostname += String(server[i]);
        
        if (i != size-1) {
            _hostname += ".";
        }
    }	   
}

bool WebSocketClient::connect() {
    if (_client.connect(_server, _port)) {
        sendHandshake();
        return readHandshake();
    }
	return false;
}

bool WebSocketClient::connected() {
    return _client.connected();
}

void WebSocketClient::disconnect() {
    _client.stop();
}

void WebSocketClient::monitor() {
	if (!_client.available()) return;
	char c = _client.read();
    String data = "";
	if (c == 0) {
		c = _client.read();
		while(c >= 0) {
			data += c;
			c = _client.read();
		}
		
		if (_dataArrivedDelegate != NULL) {
			_dataArrivedDelegate(*this, data);
		}
	}
}

void WebSocketClient::setDataArrivedDelegate(DataArrivedDelegate dataArrivedDelegate) {
	  _dataArrivedDelegate = dataArrivedDelegate;
}

bool WebSocketClient::readHandshake() {
	String handshake = "", line;
	int timeout = millis()+_timeout;
	while(!_client.available()) {
		if (millis()>timeout) {
			_client.stop();
			return false;
		}
	}
    while((line = readLine()) != "") {
        handshake += line;
    }
    return handshake.indexOf("HTTP/1.1 101 Web Socket Protocol Handshake") != -1;
}

void WebSocketClient::sendHandshake() {
    _client.print("GET ");
    _client.print(_path);
    _client.println(" HTTP/1.1");
    _client.println("Upgrade: WebSocket");
    _client.println("Connection: Upgrade");
    _client.print("Host: ");
    _client.print(_hostname);
    _client.print(":");
    _client.println(_port);
    _client.println("Origin: ArduinoWebSocketClient");
    _client.println();
}

String WebSocketClient::readLine() {
    String line = "";
    char c = 0;
    while(c != '\n') {
		if (_client.available()) {
			c = _client.read();
			if (c != '\n' && c != '\r' && c != -1) {
				line += c;
			}
		}
    }
    return line;
}

void WebSocketClient::send(String data) {
    _client.print((char)0);
	_client.print(data);
    _client.print((char)255);
}