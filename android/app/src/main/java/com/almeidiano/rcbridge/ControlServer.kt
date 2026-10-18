package com.almeidiano.rcbridge

import fi.iki.elonen.NanoHTTPD
import fi.iki.elonen.NanoHTTPD.IHTTPSession
import fi.iki.elonen.NanoHTTPD.Response
import fi.iki.elonen.NanoWSD
import fi.iki.elonen.NanoWSD.WebSocket
import fi.iki.elonen.NanoWSD.WebSocketFrame
import java.io.IOException
import java.io.InputStream
import java.net.Socket
import java.util.concurrent.atomic.AtomicInteger

/**
 * Servidor HTTP + WebSocket.
 *  - GET /      -> página de controle (assets/index.html)
 *  - WebSocket  -> cada mensagem de texto é um comando do protocolo serial ("F", "S", "V150"...)
 *                  "P" é só heartbeat e não é repassado ao Arduino.
 */
class ControlServer(
    port: Int,
    private val page: String,
    private val onCommand: (String) -> Unit,
    private val onClients: (Int) -> Unit,
) : NanoWSD(port) {

    private val clients = AtomicInteger(0)

    // Sem o algoritmo de Nagle: cada comando sai imediatamente, sem esperar agrupar pacotes
    override fun createClientHandler(finalAccept: Socket, inputStream: InputStream): ClientHandler {
        finalAccept.tcpNoDelay = true
        return super.createClientHandler(finalAccept, inputStream)
    }

    override fun serveHttp(session: IHTTPSession): Response =
        if (session.uri == "/" || session.uri == "/index.html") {
            NanoHTTPD.newFixedLengthResponse(Response.Status.OK, "text/html; charset=utf-8", page)
        } else {
            NanoHTTPD.newFixedLengthResponse(Response.Status.NOT_FOUND, NanoHTTPD.MIME_PLAINTEXT, "404")
        }

    override fun openWebSocket(handshake: IHTTPSession): WebSocket = object : WebSocket(handshake) {
        override fun onOpen() {
            onClients(clients.incrementAndGet())
        }

        override fun onClose(code: WebSocketFrame.CloseCode?, reason: String?, initiatedByRemote: Boolean) {
            onClients(clients.decrementAndGet())
            onCommand("S") // controle caiu -> para o carro
        }

        override fun onMessage(message: WebSocketFrame) {
            val cmd = message.textPayload.trim()
            if (cmd.isEmpty() || cmd == "P" || cmd.length > 8 || cmd.contains('\n')) return
            onCommand(cmd)
        }

        override fun onPong(pong: WebSocketFrame?) {}

        override fun onException(exception: IOException?) {}
    }
}
