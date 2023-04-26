#include "HAL/HAL.h"
#include "ESPAsyncWebServer.h" // 包含异步Web服务器库文件
#include "ArduinoJson.h"
#include<EEPROM.h>

AsyncWebServer server(80); // 创建WebServer对象, 端口号80
// 使用端口号80可以直接输入IP访问，使用其它端口需要输入IP:端口号访问
// 一个储存网页的数组
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
	<head>
		<meta charset="utf-8">
		<title>ESP32 Chart</title>
		<style>
			/* 设置页面背景颜色 */
			body {
				background-color: #f2f2f2;
			}

			/* 设置标题字体、颜色和居中对齐 */
			h1 {
				font-family: Arial, sans-serif;
				color: #333;
				text-align: center;
			}

			/* 设置按钮样式 */
			button {
				background-color: #4CAF50; /* 绿色 */
				border: none;
				color: white;
				padding: 10px 20px;
				text-align: center;
				text-decoration: none;
				display: inline-block;
				font-size: 16px;
				margin: 4px 2px;
				cursor: pointer;
				border-radius: 5px;
			}

			/* 设置按钮悬停时的样式 */
			button:hover {
				opacity: 0.8;
			}

			/* 设置画布样式 */
			canvas {
				margin: 10px;
				border: 1px solid #ccc;
				border-radius: 5px;
			}

			/* 设置图例样式 */
			.legend {
				display: flex;
				justify-content: center;
				margin: 10px;
			}

			/* 设置图例项样式 */
			.legend-item {
				display: flex;
				align-items: center;
				margin-right: 20px;
			}

			/* 设置图例颜色样式 */
			.legend-color {
				width: 20px;
				height: 20px;
				margin-right: 5px;
				border-radius: 50%;
			}

			/* 设置图例标签样式 */
			.legend-label {
				font-family: Arial, sans-serif;
				font-size: 14px;
				color: #333;
			}
		</style>
	</head>
	<body>
		<h1>ESP32 Chart</h1>
		<button class="button" onclick="window.location.href='/index_ill'" type="button">光照数据</button>
		<button class="button" onclick="window.location.href='/control'" type="button">控制</button>
		<button class="button" onclick="window.location.href='/index_wifi_config'" type="button">WiFi配置</button>
		<canvas id="myChart" width="400" height="100"></canvas>
		<div class="legend">
			<div class="legend-item">
				<div class="legend-color" style="background-color: rgb(75, 192, 192);"></div>
				<div class="legend-label">实时湿度曲线</div>
			</div>
			<div class="legend-item">
				<div class="legend-color" style="background-color: rgb(255, 8, 58);"></div>
				<div class="legend-label">实时光照曲线</div>
			</div>
		</div>
		<canvas id="myChart2" width="400" height="100"></canvas>
		<script src="https://cdn.jsdelivr.net/npm/chart.js@2.8.0"></script>
		<script>
			var arr=[];
			const ctx = document.getElementById('myChart');
			const labels = [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16];  // 设置 X 轴上对应的标签
			const data = {
				labels: labels,
				datasets: [{
					label: '实时湿度曲线',
					data: arr,
					fill: false,
					borderColor: 'rgb(75, 192, 192)', // 设置线的颜色
					tension: 0.3
				}],
				options: {
					responsive: true,
				}
			};
			const config = {
				type: 'line', // 设置图表类型
				data: data,
			};
			const myChart = new Chart(ctx, config);

			///////////////////////////////////////////////////////////////////////////////////////
			//var arr_hum=[];
			var arr_ill=[];
			const ctx1 = document.getElementById('myChart2');
			//const labels = [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16];  // 设置 X 轴上对应的标签
			const data1 = {
				labels: labels,
				datasets: [
					{
						label: '实时湿度曲线',
						data: arr,
						borderColor: 'rgb(75, 192, 192)',
						tension: 0.3
						//backgroundColor: Utils.transparentize(Utils.CHART_COLORS.red, 0.5),
					},
					{
						label: '实时光照曲线',
						data: arr_ill,
						borderColor: 'rgb(255, 8, 58)',
						tension: 0.3
						//backgroundColor: Utils.transparentize(Utils.CHART_COLORS.blue, 0.5),
					}
				]
			};
			const config1 = {
				type: 'line', // 设置图表类型
				data: data1,
			};
			const myChart1 = new Chart(ctx1, config1);

			setInterval(function () {
				var xhttp = new XMLHttpRequest();
				xhttp.onreadystatechange = function () {
					if (this.readyState == 4 && this.status == 200) {
						var j_obj=JSON.parse(this.responseText);
						if(arr.length>16){
							arr.push(j_obj["data"][0]);
							arr.shift();
						}else{
							arr.push(j_obj["data"][0]);
						}
						myChart.update();
						//arr.shift();
						//myChart.update();
					}
				};
				// 使用GET的方式请求 /get_data
				xhttp.open("GET", "/get_data?hum", true);
				xhttp.send();
			}, 1000)

			setInterval(function () {
				var xhttp = new XMLHttpRequest();
				xhttp.onreadystatechange = function () {
					if (this.readyState == 4 && this.status == 200) {
						var j_obj=JSON.parse(this.responseText);
						if(arr_ill.length>16){
							arr_ill.push(j_obj["data"][0]);
							arr_ill.shift();
						}else{
							arr_ill.push(j_obj["data"][0]);
						}
						myChart1.update();
						//arr.shift();
						//myChart.update();
					}
				};
				// 使用GET的方式请求 /get_data
				xhttp.open("GET", "/get_data?ill", true);
				xhttp.send();
			}, 1000)

		</script>
	</body>
</html>
)rawliteral";
const char index_wifi_config[] PROGMEM = R"rawliteral(
	<!DOCTYPE html>
<html>
<head>
  <title>ESP32 WiFi Setup</title>
  <meta charset="UTF-8">
  <style>
    body {
      font-family: Arial, sans-serif;
      background-color: #f2f2f2;
    }

    h1 {
      text-align: center;
      color: #333;
    }

    form {
      max-width: 500px;
      margin: 0 auto;
      background-color: #fff;
      padding: 20px;
      border-radius: 5px;
      box-shadow: 0px 0px 10px rgba(0,0,0,0.2);
    }

    label {
      display: block;
      margin-bottom: 5px;
      font-weight: bold;
      color: #333;
    }

    input[type="text"],
    input[type="password"] {
      display: block;
      width: 100%;
      padding: 10px;
      border-radius: 3px;
      border: 1px solid #ccc;
      margin-bottom: 10px;
    }

    select {
      display: block;
      width: 100%;
      padding: 10px;
      border-radius: 3px;
      border: 1px solid #ccc;
      margin-bottom: 10px;
      background-color: #fff;
    }

    #ipdiv {
      display: none;
    }

    #ipdiv input {
      width: 48%;
      display: inline-block;
    }

    #ipdiv label {
      width: 50%;
      display: inline-block;
      margin-bottom: 0;
    }

    input[type="submit"] {
      background-color: #333;
      color: #fff;
      border: none;
      padding: 10px 20px;
      border-radius: 3px;
      cursor: pointer;
    }

    button {
      background-color: #333;
      color: #fff;
      border: none;
      padding: 10px 20px;
      border-radius: 3px;
      cursor: pointer;
      margin-bottom: 20px;
    }
  </style>
</head>
<body>
  <h1>ESP32 WiFi Setup</h1>
  <button onclick="window.location.href='/get_wifi_config'" type="button" >查看WiFi配置</button>
  <button onclick="goBack()">返回</button>

<script>
function goBack() {
  window.history.back();
}
</script>
  <form method="post" action="/connect">
    <label for="ssid">WiFi SSID:</label>
    <input type="text" id="ssid" name="ssid"><br><br>
    <label for="password">WiFi Password:</label>
    <input type="password" id="password" name="password"><br><br>
    <label for="mode">Connection Mode:</label>
    <select id="mode" name="mode">
      <option value="0">Hotspot</option>
      <option value="1">Station</option>
    </select><br><br>
    <div id="ipdiv">
      <label for="ip">IP:</label>
      <input type="text" id="ip" name="ip">
      <label for="gateway">Gateway:</label>
      <input type="text" id="gateway" name="gateway"><br><br>
    </div>
    <input type="submit" value="Connect">
  </form>
  <script>
    document.getElementById("mode").addEventListener("change", function() {
      if (this.value == "1") {
        document.getElementById("ipdiv").style.display = "block";
      } else {
        document.getElementById("ipdiv").style.display = "none";
      }
    });
  </script>
</body>
</html>
	)rawliteral";
const char index_html_ill[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
	<head>
		<meta charset="utf-8">
	</head>
	<body>
		<h1>ESP32 Chart</h1>
        <button onclick="window.location.href='/'" type="button" >湿度数据</button>
        <button onclick="window.location.href='/control'" type="button" >控制</button>
		<button onclick="goBack()">返回</button>

<script>
function goBack() {
  window.history.back();
}
</script>
		<canvas id="myChart" width="400" height="100"></canvas>
		<script src="https://cdn.jsdelivr.net/npm/chart.js@2.8.0"></script>
		<script>
		    var arr=[];
			const ctx = document.getElementById('myChart');
			const labels = [1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16];  // 设置 X 轴上对应的标签
			const data = {
			  labels: labels,
			  datasets: [{
				label: '实时光照曲线',
				data: arr,
				fill: false,
				borderColor: 'rgb(75, 192, 192)', // 设置线的颜色
				tension: 0.3
			  }],
			options: {
				responsive: true,
			}
			};
			const config = {
			  type: 'line', // 设置图表类型
			  data: data,
			};
			const myChart = new Chart(ctx, config);
			
			setInterval(function () {
				var xhttp = new XMLHttpRequest();
				xhttp.onreadystatechange = function () {
					if (this.readyState == 4 && this.status == 200) {
						var j_obj=JSON.parse(this.responseText);
                        if(arr.length>16){
                            arr.push(j_obj["data"][0]);
                            arr.shift();
                        }else{
                            arr.push(j_obj["data"][0]);
                        }
                        myChart.update();
						//arr.shift();
						//myChart.update();
					}
				};
				// 使用GET的方式请求 /get_data
				xhttp.open("GET", "/get_data?ill", true);
				xhttp.send();
			}, 1000)
            
		</script>
	</body>
</html>
)rawliteral";
void handleGetConfig(AsyncWebServerRequest *request) {
	// 读取EEPROM中的WiFi配置
	WiFiConfig config;
	EEPROM.begin(sizeof(config));
	EEPROM.get(0, config);
	EEPROM.end();

	// 构造HTML响应
	String html = "<html><head><title>ESP32 WiFi Setup</title></head><body>";
	html += "<h1>Current WiFi Configuration</h1>";
	html += "<button onclick=\"goBack()\">Go back</button>";

	html += "<script>";
	html += "function goBack() {";
	html += " window.history.back();";
	html += "}";
	html += "</script>";
	html += "<p>SSID: " + String(config.ssid) + "</p>";
	html += "<p>Password: " + String(config.password) + "</p>";
	html += "<p>Mode: ";
	if (config.mode == 0) {
	html += "Hotspot";
	} else {
	html += "Station";
	}
	html += "</p>";
	html += "<p>IP: " + config.ip.toString() + "</p>";
	html += "<p>Gateway: " + config.gateway.toString() + "</p>";
	html += "</body></html>";

	// 返回HTML响应
	request->send(200, "text/html", html);
}
const char control_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <meta charset="utf-8">
  <title>ESP32 Control</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      margin: 0;
      padding: 0;
      background-color: #f5f5f5;
    }

    h1 {
      text-align: center;
      color: #333;
    }

    input[type="button"] {
      display: block;
      margin: 10px auto;
      padding: 10px 20px;
      border: none;
      border-radius: 5px;
      background-color: #4CAF50;
      color: #FFF;
      cursor: pointer;
      transition: background-color 0.3s ease;
    }

    input[type="button"]:hover {
      background-color: #3e8e41;
    }

    button {
      display: block;
      margin: 10px auto;
      padding: 10px 20px;
      border: none;
      border-radius: 5px;
      background-color: #008CBA;
      color: #FFF;
      cursor: pointer;
      transition: background-color 0.3s ease;
    }

    button:hover {
      background-color: #006d8c;
    }
  </style>
  <script>  
    window.onload = function(){
      var btn = document.getElementById("button");  
      var btn1 = document.getElementById("button1"); 
      var btn2 = document.getElementById("button2"); 
      var btn3 = document.getElementById("button3"); 
      var btn4 = document.getElementById("button4"); 
      var btn5 = document.getElementById("button5"); 

      btn.addEventListener('click',function() {
        var xhttp = new XMLHttpRequest();
        xhttp.open("GET", "/esp_control?10ml", true);
        xhttp.send();
      },false);  

      btn1.addEventListener('click',function() {
        var xhttp = new XMLHttpRequest();
        xhttp.open("GET", "/esp_control?20ml", true);
        xhttp.send();
      },false); 

      btn2.addEventListener('click',function() {
        var xhttp = new XMLHttpRequest();
        xhttp.open("GET", "/esp_control?30ml", true);
        xhttp.send();
      },false); 

      btn3.addEventListener('click',function() {
        var xhttp = new XMLHttpRequest();
        xhttp.open("GET", "/esp_control?50ml", true);
        xhttp.send();
      },false); 

      btn4.addEventListener('click',function() {
        var xhttp = new XMLHttpRequest();
        xhttp.open("GET", "/esp_control?relay_open", true);
        xhttp.send();
      },false); 

      btn5.addEventListener('click',function() {
        var xhttp = new XMLHttpRequest();
        xhttp.open("GET", "/esp_control?relay_close", true);
        xhttp.send();
      },false);
    }  
  </script>
</head>
<body>
  <h1>ESP32 Control</h1>
  <input  id="button" type="button" value="出水-10ML" > 
  <input  id="button1" type="button" value="出水-20ML" > 
  <input  id="button2" type="button" value="出水-30ML" > 
  <input  id="button3" type="button" value="出水-50ML" > 
  <input  id="button4" type="button" value="开水阀" > 
  <input  id="button5" type="button" value="关水阀" > 

  <button onclick="window.location.href='/index_ill'" type="button" >光照数据</button>
  <button onclick="window.location.href='/'" type="button" >湿度数据</button>

    <button onclick="goBack()">返回</button>

<script>
function goBack() {
  window.history.back();
}
</script>
</body>
</html>
)rawliteral";
void handleConnect(AsyncWebServerRequest *request) {

  // 获取表单中的WiFi账号密码和连接模式等信息
  String ssid = request->arg("ssid");
  String password = request->arg("password");
  int mode = request->arg("mode").toInt();
  String ipStr = request->arg("ip");
  String gatewayStr = request->arg("gateway");

  	Serial.println(ssid);
	Serial.println(password);

	WiFiConfig config;
	strncpy(config.ssid, ssid.c_str(), sizeof(config.ssid));
	strncpy(config.password, password.c_str(), sizeof(config.password));
	config.mode = mode;
	config.ip.fromString(ipStr);
	config.gateway.fromString(gatewayStr);
	EEPROM.begin(sizeof(config));
	EEPROM.put(0, config);
	EEPROM.commit();
	EEPROM.end();
	/*
	WiFi.disconnect();
  // 连接WiFi网络或创建热点
  if (mode == 0) {
    // 创建热点
    WiFi.softAP(ssid.c_str(), password.c_str());
  } else {
    // 连接WiFi网络
	
    WiFi.begin(ssid.c_str(), password.c_str());
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
    }
    // 设置IP和Gateway等网络参数
	IPAddress subnet(255,255,255,0);
     WiFi.config(config.ip, config.gateway,subnet);
  }*/

  // 返回设置结果
  request->send(200, "text/plain", "WiFi connected.");
}
/*
String Merge_Data(void)
{
    int Humidity = 51;    // 此处应为获取温度代码
    int Temperature = 26; // 此处应为获取湿度代码
    // 将温湿度打包为一个HTML显示代码
    String dataBuffer = "<p>";
    dataBuffer += "<h1>传感器数据 </h1>";
    dataBuffer += "<b>温度: </b>";
    dataBuffer += String(Temperature);
    dataBuffer += "<br/>";
    dataBuffer += "<b>湿度: </b>";
    dataBuffer += String(Humidity);
    dataBuffer += "<br /></p>";
    // 最后要将数组返回出去
    return dataBuffer;
}*/
void get_data(AsyncWebServerRequest *request)
{
    if (request->hasParam("hum"))
    {
        request->send_P(200, "text/plain", String("{\"data\":[" + (String)HAL::Moisture_GetValue() + "]}").c_str());
        return;
    }
    else if (request->hasParam("ill"))
    { // illumination
        request->send_P(200, "text/plain", String("{\"data\":[" + (String)HAL::ILL_GetValue() + "]}").c_str());
        return;
        // 发送接收成功标志符
    }

    // StaticJsonDocument<200> doc; //声明一个JsonDocument对象
    //  DynamicJsonDocument doc(200); //声明一个JsonDocument对象
    // DynamicJsonDocument doc(128);
    // doc["data"][0]=(HAL::HUM_GetValue());
    // if(doc["data"].size()>16)doc["data"].clear();
    // Serial.println("ppppppppppppppppppppppppppppp");
    // Serial.println(doc.as<String>());
    // return doc.as<String>().c_str();
    // serializeJson(doc, Serial);
    //  This prints:
    //  {"sensor":"gps","time":1351824120,"data":[48.756080,2.302038]}
    // return doc.as<String>();
}
void esp_control(AsyncWebServerRequest *request)
{
    if (request->hasParam("10ml"))
    {
        HAL::Relay_Open_ML(10);
    }
    else if (request->hasParam("20ml"))
    {
        HAL::Relay_Open_ML(20);
    }
    else if (request->hasParam("30ml"))
    {
        HAL::Relay_Open_ML(30);
    }
    else if (request->hasParam("50ml"))
    {
        HAL::Relay_Open_ML(50);
    }
    else if (request->hasParam("relay_open"))
    {
        HAL::Relay_Open_ML(102400);
    }
    else if (request->hasParam("relay_close"))
    {
        HAL::Relay_Close();
    }
    else
    {
        request->send(401);
        return;
    }
    request->send(200);
}
// 下发处理回调函数
void Config_Callback(AsyncWebServerRequest *request)
{
    if (request->hasParam("value")) // 如果有值下发
    {
        String HTTP_Payload = request->getParam("value")->value();    // 获取下发的数据
        Serial.printf("[%lu]%s\r\n", millis(), HTTP_Payload.c_str()); // 打印调试信息
    }
    request->send(200, "text/plain", "OK"); // 发送接收成功标志符
}
void HAL::WebServer_Init()
{
    // get_data();
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "text/html", index_html); });
    // 设置反馈的信息，在HTML请求这个Ip/dht这个链接时，返回打包好的传感器数据
    server.on("/index_ill", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "text/html", index_html_ill); });
	server.on("/index_wifi_config", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "text/html", index_wifi_config); });

	server.on("/get_wifi_config", HTTP_GET, handleGetConfig);


	server.on("/connect", HTTP_POST, handleConnect);
    server.on("/get_data", HTTP_GET, get_data);

    server.on("/control", HTTP_GET, [](AsyncWebServerRequest *request)
              { request->send_P(200, "text/html", control_html); });
    server.on("/esp_control", HTTP_GET, esp_control);

    /*
        server.on("/get_data", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send_P(200, "text/plain", get_data().c_str()); });


        server.on("/dht", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send_P(200, "text/plain", Merge_Data().c_str()); });
        server.on("/set", HTTP_GET, Config_Callback); // 绑定配置下发的处理函数
        */

    server.begin(); // 初始化HTTP服务器
	Serial.println("Server OK");
}