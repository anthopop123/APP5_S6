<template>
  <div>
    <h1>Serveur</h1>
    <h2>
      Historique d'entré et de sortie
    </h2>
    <table class="tableES">
      <tr>
        <th>id</th>
        <th>name</th>
        <th>date/heure</th>
        <th>Local</th>
      </tr>
      <tr v-for="(entry,index) of entries" :key="index">
        <td>{{ entry.id }}</td>
        <td>{{ entry.name }}</td>
        <td>{{ entry.date }}</td>
        <td>{{ entry.local }}</td>
      </tr>
    </table>
    <br>
    <br>

    <div class="buttonBox">
      <label for="led">Toggle led btn</label>
      <div>
        <button style="margin-right:15px" id="led" @click="toggleLed('on')">Allumer la led</button>
        <button id="led" @click="toggleLed('off')">Éteindre la led</button>
      </div>
    </div>
  </div>
</template>
<script>
export default {
  data() {
    return {
      connection: {
        protocol: "mqtt",
        host: "localhost",
        port: 15675,
        clean: true, // Reserved session
        connectTimeout: 4000, // Time out
        reconnectPeriod: 4000, // Reconnection interval
        // Certification Information
        clientId: "mqttjs_3be2c321",
        username: "emqx_test",
        password: "emqx_test",
      },
      subscription: {
        topic: "topic/mqttx",
        qos: 0,
      },
      publish: {
        topic: "hello/world",
        qos: 0,
        msg: "Hello, Iam browser.",
        payload: '{"msg": "Hello, Iam browser."}',
      },
      receiveNews: "",
      qosList: [
        { label: 0, value: 0 },
        { label: 1, value: 1 },
        { label: 2, value: 2 },
      ],
      client: {
        connected: false,
      },
      subscribeSuccess: false,
      entries: [{
        id: 1,
        name: "Jean",
        date: "01/01/2023 12:00:00",
        local: "c1-3014",
      }],
    };
  },
  watch: {
    publish() {
      this.publish.payload = '{"msg": "' + this.publish.msg + '"}';
    },
  },
  methods: {
    // Create connection
    initData() {
      this.client = {
        connected: false,
      };
      this.retryTimes = 0;
      this.connecting = false;
      this.subscribeSuccess = false;
    },
    handleOnReConnect() {
      this.retryTimes += 1;
      if (this.retryTimes > 5) {
        try {
          this.client.end();
          this.initData();
          this.$message.error("Connection maxReconnectTimes limit, stop retry");
        } catch (error) {
          this.$message.error(error.toString());
        }
      }
    },
    createConnection() {
      try {
        const mqtt = require("mqtt");
        console.log("trying to connect");
        var options = {
          clientId: "web-client",
          connectTimeout: 5000,
          hostname: "localhost",
          port: 8883,
          path: "/mqtt",
        };

        this.client = mqtt.connect(options);

        this.client.on("connect", () => {
          console.log("Connected");
          console.log(this.client);
        });
        this.client.on("message",(msg)=>{
          console.log(msg);
          this.entries.push({
            id: msg.id,
            date: new Date(),
            local: msg.local,
          });
        });
      } catch (error) {
        this.connecting = false;
        console.log("mqtt.connect error", error);
      }
    },

    doSubscribe() {
      const { topic, qos } = this.subscription;
      this.client.subscribe(topic, { qos }, (error, res) => {
        if (error) {
          console.log("Subscribe to topics error", error);
          return;
        }
        this.subscribeSuccess = true;
        console.log("Subscribe to topics res", res);
      });
    },

    doPublish() {
      const topic = this.publish.topic;
      const qos = 0;
      const payload = this.publish.payload;
      console.log(topic);
      this.client.publish(topic, payload, qos, (error) => {
        if (error) {
          console.log("Publish error", error);
        }
      });
    },

    destroyConnection() {
      if (this.client.connected) {
        try {
          this.client.end();
          this.client = {
            connected: false,
          };
          console.log("Successfully disconnected!");
        } catch (error) {
          console.log("Disconnect failed", error.toString());
        }
      }
    },
    toggleLed(x) {
      console.log("toggleLed");
      fetch("http://192.168.2.30/"+x, {mode: 'no-cors'});
    },
  },
  mounted() {
    this.createConnection();
    this.doSubscribe();
  },
  beforeUnmount() {
    this.destroyConnection();
  },
};
</script>

<style>
#app {
  font-family: Avenir, Helvetica, Arial, sans-serif;
  -webkit-font-smoothing: antialiased;
  -moz-osx-font-smoothing: grayscale;
  text-align: center;
  color: #2c3e50;
  margin-top: 60px;
}
.buttonBox {
  display: flex;
  flex-direction: column;
  align-items: center;
  margin: auto;

}
.buttonBox label {
  margin-bottom: 10px;
  font-weight: bold;
  font-size: 20px;
}
button{
  background: black;
  color: white;
  border: 1px solid white;
  border-radius: 20px;
  padding: 10px 20px;
  font-size: 16px;
  cursor: pointer;
}
button:hover{
  background: white;
  color: black;
  border: 1px solid black;
}
.tableES {
  margin: auto;
  border: 1px solid black;
  border-collapse: collapse;
}
.tableES th {
  border: 1px solid black;
  padding: 5px;
}
.tableES td {
  border: 1px solid black;
  padding: 5px;
}
</style>
