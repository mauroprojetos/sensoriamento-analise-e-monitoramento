import mqtt from 'mqtt'
import knex from 'knex'

const client = mqtt.connect(`mqtt://192.168.0.20`)
const mqttTopic = 'douglaszuqueto/casa_01/cozinha/temperatura/sensor_01'

const db = knex({
  client: 'mysql',
  connection: {
    host: '192.168.0.20',
    user: 'root',
    password: 'root',
    database: 'sensor'
  },
  pool: {min: 2, max: 7}
})

client.on('connect', () => {
  console.log(`Connection successfully to 192.168.0.20`)
  client.subscribe(mqttTopic)
})

client.on('message', (topic, message) => {
  if (mqttTopic !== topic) {
    return
  }
  const temperature = message.toString()

  create('Sensor 01', temperature)
  create('Sensor 02', parseFloat(temperature) + 1)
  create('Sensor 03', parseFloat(temperature) + 2)
})

const create = (topic, temperature) => {
  const payload = {
    name: topic,
    alias: topic,
    temperature,
    moisture: (temperature * 100) / 50
  }
  db.table('sensors').insert(payload)
    .then((id) => {
      console.log(id)
    })
    .catch((e) => {
      console.log(e.message)
    })
}
