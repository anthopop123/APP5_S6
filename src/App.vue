<template>
  <div>
    <h1>sstfu</h1>
    <button @click="doPublish">
      envoyer qqch
    </button>
  </div>
</template>
<script>

export default {
  data() {
    return {
      connection: {
        protocol:"mqtt",
        host: 'localhost',
        port: 15675,
        clean: true, // Reserved session
        connectTimeout: 4000, // Time out
        reconnectPeriod: 4000, // Reconnection interval
        // Certification Information
        clientId: 'mqttjs_3be2c321',
        username: 'emqx_test',
        password: 'emqx_test',
      },
      subscription: {
        topic: 'topic/mqttx',
        qos: 0,
      },
      publish: {
        topic: 'hello/world',
        qos: 0,
        payload: '{ "msg": "Hello, I am browser." }',
      },
      receiveNews: '',
      qosList: [
        { label: 0, value: 0 },
        { label: 1, value: 1 },
        { label: 2, value: 2 },
      ],
      client: {
        connected: false,
      },
      subscribeSuccess: false,
    }
  },

  methods: {
    // Create connection
    initData() {
      this.client = {
        connected: false,
      }
      this.retryTimes = 0
      this.connecting = false
      this.subscribeSuccess = false
    },
    handleOnReConnect() {
      this.retryTimes += 1
      if (this.retryTimes > 5) {
        try {
          this.client.end()
          this.initData()
          this.$message.error('Connection maxReconnectTimes limit, stop retry')
        } catch (error) {
          this.$message.error(error.toString())
        }
      }
    },
    createConnection() {
      try {
        const mqtt = require('mqtt')
        console.log('trying to connect')
        var options = {
          clientId: 'web-client',
          connectTimeout: 5000,
          hostname: 'localhost',
          port: 8883,
          path: '/mqtt'
        };

        this.client = mqtt.connect(options);


        
        this.client.on('connect', () => {
          console.log('Connected')
          console.log(this.client)
          //client.end()
        })
      } catch (error) {
        this.connecting = false
        console.log('mqtt.connect error', error)
      }
    },

    doSubscribe() {
      const { topic, qos } = this.subscription
      this.client.subscribe(topic, { qos }, (error, res) => {
        if (error) {
          console.log('Subscribe to topics error', error)
          return
        }
        this.subscribeSuccess = true
        console.log('Subscribe to topics res', res)
      })
    },

    doPublish() {
      const topic= 'hello/world'
      const qos= 0
      const payload= '{ "msg": "Hello, I am browser." }'
      console.log(topic);
      this.client.publish(topic, payload, qos, error => {
        if (error) {
          console.log('Publish error', error)
        }
      })
    },

    destroyConnection() {
      if (this.client.connected) {
        try {
          this.client.end()
          this.client = {
            connected: false,
          }
          console.log('Successfully disconnected!')
        } catch (error) {
          console.log('Disconnect failed', error.toString())
        }
      }
    }
  },
  mounted(){
    this.createConnection();
  }
}
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
</style>
