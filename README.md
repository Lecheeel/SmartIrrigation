# SmartIrrigation
## 智能灌溉系统

本项目是一个基于ESP32的智能灌溉系统，通过使用多种传感器和API，实现对环境条件的监测和相应的灌溉控制，以优化植物生长的水分利用效率。

### 项目结构

```
SmartIrrigation/
│
├─ src/
│  ├─ Configs/
│  │  ├─ Config.h          # 配置文件
│  │  └─ Version.h         # 版本信息
│  ├─ HAL/
│  │  ├─ CommonMacro.h     # 通用宏定义
│  │  ├─ HAL.cpp           # 硬件抽象层实现
│  │  ├─ HAL.h             # 硬件抽象层头文件
│  │  ├─ HAL_Air_Sensor.cpp       # 空气传感器控制
│  │  ├─ HAL_I2C_Scan.cpp         # I2C扫描功能
│  │  ├─ HAL_Illumination.cpp     # 光照传感器控制
│  │  ├─ HAL_Network.cpp          # 网络控制
│  │  ├─ HAL_Relay.cpp            # 继电器控制
│  │  ├─ HAL_Serial.cpp           # 串口通信
│  │  ├─ HAL_Soil_Moisture_Sensor.cpp  # 土壤湿度传感器控制
│  │  ├─ HAL_WeatherAPI.cpp       # 天气API控制
│  │  └─ HAL_WebServer.cpp        # Web服务器控制
│  └─ main.cpp          # 主程序文件
└─ test/
   └─ README            # 测试说明文档
```

### 主要功能

- **环境监测**：系统利用各类传感器监测土壤湿度、空气质量、光照强度等环境参数。
- **自动化灌溉**：根据实时环境数据，系统自动调整灌溉控制，以优化植物生长所需的水分。
- **远程控制**：通过网络连接，用户可以远程监测和控制灌溉系统，实现智能化管理。

### 作者

- [Lecheeel](https://github.com/Lecheeel)

### 许可证

本项目采用 [GNU许可证](LICENSE) 进行许可。