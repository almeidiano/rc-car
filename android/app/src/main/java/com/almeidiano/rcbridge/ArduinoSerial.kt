package com.almeidiano.rcbridge

import android.app.PendingIntent
import android.content.Context
import android.content.Intent
import android.hardware.usb.UsbManager
import android.os.Build
import com.hoho.android.usbserial.driver.UsbSerialPort
import com.hoho.android.usbserial.driver.UsbSerialProber
import com.hoho.android.usbserial.util.SerialInputOutputManager
import java.io.IOException

/**
 * Conexão USB OTG com o Arduino Uno (115200 8N1).
 * Envia uma linha por comando (ex: "F\n") e repassa as linhas recebidas.
 */
class ArduinoSerial(
    private val context: Context,
    private val onLine: (String) -> Unit,
    private val onStatus: (String) -> Unit,
) : SerialInputOutputManager.Listener {

    companion object {
        const val ACTION_USB_PERMISSION = "com.almeidiano.rcbridge.USB_PERMISSION"
        private const val BAUD_RATE = 115200
        private const val WRITE_TIMEOUT_MS = 50
    }

    private val usbManager = context.getSystemService(Context.USB_SERVICE) as UsbManager
    private var port: UsbSerialPort? = null
    private var ioManager: SerialInputOutputManager? = null
    private val rxBuffer = StringBuilder()

    @Synchronized
    fun connect() {
        if (port != null) return

        val driver = UsbSerialProber.getDefaultProber().findAllDrivers(usbManager).firstOrNull()
        if (driver == null) {
            onStatus("Arduino não encontrado — conecte o cabo OTG")
            return
        }

        if (!usbManager.hasPermission(driver.device)) {
            val intent = Intent(ACTION_USB_PERMISSION).setPackage(context.packageName)
            val flags = if (Build.VERSION.SDK_INT >= 31) PendingIntent.FLAG_MUTABLE else 0
            usbManager.requestPermission(driver.device, PendingIntent.getBroadcast(context, 0, intent, flags))
            onStatus("Aguardando permissão USB…")
            return
        }

        val connection = usbManager.openDevice(driver.device)
        if (connection == null) {
            onStatus("Falha ao abrir o dispositivo USB")
            return
        }

        val newPort = driver.ports[0]
        try {
            newPort.open(connection)
            newPort.setParameters(BAUD_RATE, 8, UsbSerialPort.STOPBITS_1, UsbSerialPort.PARITY_NONE)
            newPort.setDTR(true) // O Uno reinicia aqui; ele avisa "RC-CAR PRONTO" quando terminar
        } catch (e: IOException) {
            try { newPort.close() } catch (_: IOException) {}
            onStatus("Erro ao abrir a serial: ${e.message}")
            return
        }

        port = newPort
        ioManager = SerialInputOutputManager(newPort, this).also { it.start() }
        onStatus("USB conectado (${driver.javaClass.simpleName}), aguardando o Arduino iniciar…")
    }

    @Synchronized
    fun send(command: String) {
        val p = port ?: return
        try {
            p.write("$command\n".toByteArray(), WRITE_TIMEOUT_MS)
        } catch (e: IOException) {
            disconnect("Erro ao enviar: ${e.message}")
        }
    }

    @Synchronized
    fun disconnect(reason: String) {
        if (port == null) return
        ioManager?.stop()
        ioManager = null
        try { port?.close() } catch (_: IOException) {}
        port = null
        rxBuffer.clear()
        onStatus(reason)
    }

    override fun onNewData(data: ByteArray) {
        for (b in data) {
            val c = b.toInt().toChar()
            if (c == '\n' || c == '\r') {
                if (rxBuffer.isNotEmpty()) {
                    onLine(rxBuffer.toString())
                    rxBuffer.clear()
                }
            } else {
                rxBuffer.append(c)
            }
        }
    }

    override fun onRunError(e: Exception) {
        disconnect("Conexão USB perdida: ${e.message}")
    }
}
