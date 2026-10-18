package com.almeidiano.rcbridge

import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale

/** Estado compartilhado entre o serviço (ponte) e a tela. */
object BridgeState {
    var usbStatus = "Desconectado"
        private set
    var clients = 0
        private set

    private val logLines = ArrayDeque<String>()
    private val timeFormat = SimpleDateFormat("HH:mm:ss", Locale.US)

    @Volatile
    var listener: (() -> Unit)? = null

    fun setUsb(status: String) {
        synchronized(this) { usbStatus = status }
        log(status)
    }

    fun setClients(count: Int) {
        synchronized(this) { clients = count }
        listener?.invoke()
    }

    fun log(line: String) {
        synchronized(this) {
            logLines.addLast("${timeFormat.format(Date())}  $line")
            while (logLines.size > 100) logLines.removeFirst()
        }
        listener?.invoke()
    }

    fun logText(): String = synchronized(this) { logLines.joinToString("\n") }
}
