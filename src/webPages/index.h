const char index_html[] PROGMEM = R"=====(
<HTML>

<HEAD>
    <TITLE>home - rfid-watchdog</TITLE>
</HEAD>

<BODY>
    <h1>rfid-watchdog maintaince page</h1>
    <h2>version: <span id="versionTag">n/a</span></h2>
    <hr>
    <ul>
        <li><a href="config.html">Config</a></li>
        <li><a href="cache.html">Cache</a></li>
    </ul>
    <script>
        let req = new XMLHttpRequest();
        req.open("GET", "/info/version");
        req.onload = () => {
            document.getElementById("versionTag").innerText = req.responseText;
        };
        req.send();
    </script>
</BODY>

</HTML>
)=====";