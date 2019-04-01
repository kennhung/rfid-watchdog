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

        req.open("GET", "/cache/list");
        req.onload = () => {
            console.log(req.responseText);
            const data = JSON.parse(req.responseText);
            for (let i=0;i<data.length;i++) {
                let uid = data[i];
                let li = document.createElement("li");
                li.appendChild(document.createTextNode(uid));
                document.getElementById("cacheList").appendChild(li);
            }
        };
        req.send();
    </script>

</BODY>

</HTML>
)=====";