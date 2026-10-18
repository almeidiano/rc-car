package com.almeidiano.rcbridge

import android.app.Notification
import android.app.NotificationChannel
import android.app.NotificationManager
import android.app.PendingIntent
import android.app.Service
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.content.pm.ServiceInfo
import android.hardware.usb.UsbManager
import android.net.wifi.WifiManager
import android.os.Build
import android.os.IBinder
import android.os.PowerManager
import java.io.IOException

/**
 * Serviço em primeiro plano que mantém a ponte WiFi -> USB rodando
 * mesmo com a tela apagada.
 */
class BridgeService : Service() {

    companion object {
        const val PORT = 8080
        private const val SOCKET_TIMEOUT_MS = 5000 // a página manda heartbeat a cada 1 s
        private const val CHANNEL_ID = "bridge"
    }

    private lateinit var serial: ArduinoSerial
    private var server: ControlServer? = null
    private var wakeLock: PowerManager.WakeLock? = null
    private var wifiLock: WifiManager.WifiLock? = null

    private val usbReceiver = object : BroadcastReceiver() {
        override fun onReceive(context: Context, intent: Intent) {
            when (intent.action) {
                ArduinoSerial.ACTION_USB_PERMISSION ->
                    if (intent.getBooleanExtra(UsbManager.EXTRA_PERMISSION_GRANTED, false)) serial.connect()
                    else BridgeState.setUsb("Permissão USB negada")

                UsbManager.ACTION_USB_DEVICE_DETACHED -> serial.disconnect("Cabo USB desconectado")
            }
        }
    }

    override fun onCreate() {
        super.onCreate()
        startInForeground()

        serial = ArduinoSerial(
            this,
            onLine = { line -> if (!line.startsWith("OK")) BridgeState.log("Arduino: $line") },
            onStatus = BridgeState::setUsb,
        )

        val filter = IntentFilter().apply {
            addAction(ArduinoSerial.ACTION_USB_PERMISSION)
            addAction(UsbManager.ACTION_USB_DEVICE_DETACHED)
        }
        registerReceiver(usbReceiver, filter, if (Build.VERSION.SDK_INT >= 33) RECEIVER_NOT_EXPORTED else 0)

        // Mantém CPU e WiFi acordados com a tela apagada (evita picos de latência)
        wakeLock = (getSystemService(POWER_SERVICE) as PowerManager)
            .newWakeLock(PowerManager.PARTIAL_WAKE_LOCK, "rcbridge:cpu")
            .apply { acquire() }
        @Suppress("DEPRECATION")
        val wifiMode = if (Build.VERSION.SDK_INT >= 29) WifiManager.WIFI_MODE_FULL_LOW_LATENCY
        else WifiManager.WIFI_MODE_FULL_HIGH_PERF
        wifiLock = (applicationContext.getSystemService(WIFI_SERVICE) as WifiManager)
            .createWifiLock(wifiMode, "rcbridge:wifi")
            .apply { acquire() }

        val page = assets.open("index.html").bufferedReader().use { it.readText() }
        server = ControlServer(PORT, page, onCommand = serial::send, onClients = BridgeState::setClients).also {
            try {
                it.start(SOCKET_TIMEOUT_MS, false)
                BridgeState.log("Servidor iniciado na porta $PORT")
            } catch (e: IOException) {
                BridgeState.log("Falha ao iniciar o servidor: ${e.message}")
            }
        }
    }

    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        serial.connect()
        return START_STICKY
    }

    override fun onDestroy() {
        server?.stop()
        serial.send("S")
        serial.disconnect("Serviço encerrado")
        unregisterReceiver(usbReceiver)
        wifiLock?.release()
        wakeLock?.release()
        super.onDestroy()
    }

    override fun onBind(intent: Intent?): IBinder? = null

    private fun startInForeground() {
        val nm = getSystemService(NotificationManager::class.java)
        nm.createNotificationChannel(NotificationChannel(CHANNEL_ID, "RC Bridge", NotificationManager.IMPORTANCE_LOW))

        val openApp = PendingIntent.getActivity(
            this, 0, Intent(this, MainActivity::class.java), PendingIntent.FLAG_IMMUTABLE,
        )
        val notification = Notification.Builder(this, CHANNEL_ID)
            .setContentTitle("RC Bridge ativo")
            .setContentText("Servidor na porta $PORT")
            .setSmallIcon(android.R.drawable.stat_sys_upload)
            .setContentIntent(openApp)
            .setOngoing(true)
            .build()

        if (Build.VERSION.SDK_INT >= 29) {
            startForeground(1, notification, ServiceInfo.FOREGROUND_SERVICE_TYPE_CONNECTED_DEVICE)
        } else {
            startForeground(1, notification)
        }
    }
}
