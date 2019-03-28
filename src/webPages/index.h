const char index_html[] PROGMEM = R"=====(
<HTML>

<HEAD>
    <TITLE>rfid-watchdog</TITLE>
</HEAD>

<BODY>
    <form action="/config/save" method="POST">
        <h1>Network settings</h1>
        Wifi Mode: <br>
        <input type="radio" name="wifiMode" value="0">Station <br>
        <input type="radio" name="wifiMode" value="1">AP <br>
        <h2>Station</h2>
        SSID: <input name="statSSID"> <br>
        Password: <input name="statPass"> <br>
        <h2>AP Mode</h2>
        SSID: <input name="apSSID"> <br>
        Password: <input name="apPass"> <br>
        <h1>Watchdog server settings</h1>
        Server IP: <input name="watchdogServerIP"> <br>
        <hr>
        <button type="submit">Save</button>
    </form>

    <script>
        let req = new XMLHttpRequest();

        req.open("GET", "/config/get");
        req.onload = () => {
            
        };
    </script>

</BODY>

</HTML>
)=====";