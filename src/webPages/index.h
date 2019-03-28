const char index_html[] PROGMEM = R"=====(
<HTML>

<HEAD>
    <TITLE>rfid-watchdog</TITLE>
</HEAD>

<BODY>
    <form action="/config/save" method="POST">
        <h1>Config</h1>
        <textarea id="configInput" name="config"></textarea>
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