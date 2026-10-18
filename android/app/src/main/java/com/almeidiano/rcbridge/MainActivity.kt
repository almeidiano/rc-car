package com.almeidiano.rcbridge

import android.Manifest
import android.app.Activity
import android.content.Intent
import android.content.pm.PackageManager
import android.graphics.Typeface
import android.os.Build
import android.os.Bundle
import android.view.WindowManager
import android.widget.Button
import android.widget.LinearLayout
import android.widget.ScrollView
import android.widget.TextView
import java.net.Inet4Address
import java.net.NetworkInterface

class MainActivity : Activity() {

    private lateinit var statusView: TextView
    private lateinit var logView: TextView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON)

        if (Build.VERSION.SDK_INT >= 33 &&
            checkSelfPermission(Manifest.permission.POST_NOTIFICATIONS) != PackageManager.PERMISSION_GRANTED
        ) {
            requestPermissions(arrayOf(Manifest.permission.POST_NOTIFICATIONS), 0)
        }

        val pad = (16 * resources.displayMetrics.density).toInt()
        statusView = TextView(this).apply { textSize = 18f }
        logView = TextView(this).apply {
            typeface = Typeface.MONOSPACE
            textSize = 12f
        }

        val buttons = LinearLayout(this).apply {
            addView(Button(context).apply {
                text = "Reconectar Arduino"
                setOnClickListener { startBridge() }
            })
            addView(Button(context).apply {
                text = "Encerrar"
                setOnClickListener {
                    stopService(Intent(this@MainActivity, BridgeService::class.java))
                    finish()
                }
            })
        }

        setContentView(LinearLayout(this).apply {
            orientation = LinearLayout.VERTICAL
            setPadding(pad, pad, pad, pad)
            addView(statusView)
            addView(buttons)
            addView(
                ScrollView(context).apply { addView(logView) },
                LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, 0, 1f),
            )
        })

        startBridge()
    }

    // Chamado quando o Arduino é plugado com o app já aberto
    override fun onNewIntent(intent: Intent) {
        super.onNewIntent(intent)
        startBridge()
    }

    override fun onResume() {
        super.onResume()
        BridgeState.listener = { runOnUiThread(::render) }
        render()
    }

    override fun onPause() {
        BridgeState.listener = null
        super.onPause()
    }

    private fun startBridge() {
        startForegroundService(Intent(this, BridgeService::class.java))
    }

    private fun render() {
        statusView.text = buildString {
            appendLine("USB: ${BridgeState.usbStatus}")
            appendLine("Controles conectados: ${BridgeState.clients}")
            appendLine()
            appendLine("No celular controle, abra:")
            localAddresses().forEach { appendLine("  http://$it:${BridgeService.PORT}") }
        }
        logView.text = BridgeState.logText()
    }

    private fun localAddresses(): List<String> =
        NetworkInterface.getNetworkInterfaces().toList()
            .filter { it.isUp && !it.isLoopback }
            .flatMap { it.inetAddresses.toList() }
            .filterIsInstance<Inet4Address>()
            .mapNotNull { it.hostAddress }
}
