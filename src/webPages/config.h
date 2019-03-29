const char config_html[] PROGMEM = R"=====(
<HTML>

<HEAD>
    <TITLE>config - rfid-watchdog</TITLE>
</HEAD>

<BODY>
    <form action="/config/save" method="POST">
        <h1>Config</h1>
        <textarea id="configInput" name="config" rows="4" cols="50"></textarea>
        <hr>
        <button type="submit">Save</button>
    </form>

    <script>
        let req = new XMLHttpRequest();

        req.open("GET", "/config/get");
        req.onload = () => {
            document.getElementById("configInput").appendChild(document.createTextNode(req.responseText))
        };
        req.send();
    </script>

</BODY>

</HTML>
)=====";