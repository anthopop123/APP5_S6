<template>
  <div>
    <h1>Serveur de controle et d'archive Beacon BLE</h1>
    <h2 v-if="entriesId.lenght > 0">Historique d'entré et de sortie</h2>
    <table class="tableES" v-if="entriesId.lenght > 0">
      <tr>
        <th>id</th>
        <th>date/heure</th>
        <th>Local</th>
      </tr>
      <tr v-for="(entryId, index) of entriesId" :key="index">
        <td>{{ entryId.id }}</td>
        <td>{{ entryId.date }}</td>
        <td>{{ entryId.local }}</td>
      </tr>
    </table>
    <h2 v-if="entries.lenght > 0">Historique d'entré et de sortie avec noms</h2>
    <table class="tableES" v-if="entries.lenght > 0">
      <tr>
        <th>id</th>
        <th>name</th>
        <th>date/heure</th>
        <th>Local</th>
      </tr>
      <tr v-for="(entry, index) of entries" :key="index">
        <td>{{ entry.id }}</td>
        <td>{{ entry.name }}</td>
        <td>{{ entry.date }}</td>
        <td>{{ entry.local }}</td>
      </tr>
    </table>
    <br />
    <br />
    <div class="buttonBox">
      <label for="led">Toggle led btn</label>
      <div>
        <button style="margin-right: 15px" id="led" @click="toggleLed('on')">
          Allumer la led
        </button>
        <button id="led" @click="toggleLed('off')">Éteindre la led</button>
      </div>
    </div>
  </div>
</template>
<script>
export default {
  data() {
    return {
      entries: [
      ],
      entriesId: [
      ],
    };
  },
  methods: {
    toggleLed(x) {
      fetch("http://192.168.2.30/" + x, { mode: "no-cors" });
    },
    getIdList() {
      fetch("http://localhost:4567/idlist")
        .then((res) => res.json())
        .then((data) => {
          this.entriesId = data;
        });
    },
    getNameList() {
      fetch("http://localhost:4567/namelist")
        .then((res) => res.json())
        .then((data) => {
          this.entries = data;
        });
    },
  },
  mounted() {
    setInterval(() => {
      this.getNameList();
      this.getIdList();
    }, 1000);
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
button {
  background: black;
  color: white;
  border: 1px solid white;
  border-radius: 20px;
  padding: 10px 20px;
  font-size: 16px;
  cursor: pointer;
}
button:hover {
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
