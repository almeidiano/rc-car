plugins {
    id("com.android.application")
    id("org.jetbrains.kotlin.android")
}

android {
    namespace = "com.almeidiano.rcbridge"
    compileSdk = 35

    defaultConfig {
        applicationId = "com.almeidiano.rcbridge"
        minSdk = 26
        targetSdk = 35
        versionCode = 1
        versionName = "1.0"
    }

    // A página de controle fica em <repo>/web e é empacotada direto como asset
    sourceSets["main"].assets.srcDir("../../web")

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_17
        targetCompatibility = JavaVersion.VERSION_17
    }
    kotlinOptions {
        jvmTarget = "17"
    }
}

dependencies {
    implementation("com.github.mik3y:usb-serial-for-android:3.8.1")
    implementation("org.nanohttpd:nanohttpd-websocket:2.3.1")
}
