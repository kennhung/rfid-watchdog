const char cache_html[] PROGMEM = R"=====(
<HTML>

<HEAD>
    <TITLE>cache - rfid-watchdog</TITLE>
</HEAD>

<BODY>
    <h1>Cache</h1>

    <ol id="cacheList">

    </ol>

    <script>
        let req = new XMLHttpRequest();

        req.open("GET", "/cache/get");
        req.onload = () => {
            console.log(req.responseText);
            document.getElementById("cacheList").appendChild();
        };
        req.send();
    </script>

</BODY>

</HTML>
)=====";