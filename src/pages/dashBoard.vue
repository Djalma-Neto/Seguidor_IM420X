<template>
  <div class="q-ma-md q-gutter-md">
    <q-tabs
      v-model="tab"
      class="text-teal"
    >
      <q-tab name="all" icon="dashboard" label="all" />
      <q-tab name="velocidade" icon="speed" label="Velocidade" />
      <q-tab name="motor" icon="settings" label="Motor" />
      <q-tab name="rodas" icon="tire_repair" label="Rodas" />
    </q-tabs>

    <div class="text-h5 q-mb-md flex flex-center">Indicativos</div>

    <q-tab-panels
      v-model="tab"
      animated
      swipeable
      horizontal
    >
      <q-tab-panel name="all" class="flex flex-center q-gutter-sm">
        <!-- card de velocidade -->
        <q-card class="flex flex-center q-pa-sm cardPadrao">
          <q-card-section class="full-width flex flex-center q-pa-none">
            <div>Velocidade</div>
          </q-card-section>

          <q-separator class="full-width" blue inset />

          <q-card-section class="full-width flex flex-center q-pa-none">
            <q-knob disable
              show-value
              class="text-light-blue q-ma-md"
              v-model="velocidade"
              size="3rem"
              color="light-blue"
            />
          </q-card-section>
        </q-card>

        <!-- card RPM -->
        <q-card class="flex flex-center q-pa-sm cardPadrao">
          <q-card-section class="full-width flex flex-center q-pa-none">
            <div>Motor(RPM)</div>
          </q-card-section>

          <q-separator class="full-width" blue inset />

          <q-card-section class="full-width flex flex-center q-pa-none">
            <q-knob disable
              show-value
              class="text-red q-ma-md"
              v-model="rpm"
              size="3rem"
              color="red"
            />
          </q-card-section>
        </q-card>

        <!-- card Encoders -->
        <q-card class="flex flex-center q-pa-sm cardPadrao">
          <q-card-section class="full-width flex flex-center q-pa-none">
            <div>Rodas</div>
          </q-card-section>

          <q-separator class="full-width" blue inset />

          <q-card-section class="flex flex-center q-pa-none q-gutter-x-md">
            <q-knob disable
              show-value
              class="text-light-green-6 q-my-sm"
              v-model="Di"
              size="4rem"
              color="light-green-6"
            >
              <div>Di : {{Di}}</div>
            </q-knob>
            <q-knob disable
              show-value
              class="text-lime-6 q-my-sm"
              v-model="Es"
              size="4rem"
              color="lime-6"
            >
              <div>Es : {{Es}}</div>
            </q-knob>
          </q-card-section>
        </q-card>
      </q-tab-panel>

      <!-- apenas velocidade -->
      <q-tab-panel name="velocidade" class="flex flex-center q-gutter-sm">
        <!-- card de velocidade -->
        <q-card class="flex flex-center q-pa-sm cardPadrao">
          <q-card-section class="full-width flex flex-center q-pa-none">
            <div>Velocidade</div>
          </q-card-section>

          <q-separator class="full-width" blue inset />

          <q-card-section class="full-width flex flex-center q-pa-none">
            <q-knob disable
              show-value
              class="text-light-blue q-ma-md"
              v-model="velocidade"
              size="3rem"
              color="light-blue"
            />
          </q-card-section>
        </q-card>
      </q-tab-panel>

      <!-- apenas motor -->
      <q-tab-panel name="motor" class="flex flex-center q-gutter-sm">
       <!-- card RPM -->
        <q-card class="flex flex-center q-pa-sm cardPadrao">
          <q-card-section class="full-width flex flex-center q-pa-none">
            <div>Motor(RPM)</div>
          </q-card-section>

          <q-separator class="full-width" blue inset />

          <q-card-section class="full-width flex flex-center q-pa-none">
            <q-knob disable
              show-value
              class="text-red q-ma-md"
              v-model="rpm"
              size="3rem"
              color="red"
            />
          </q-card-section>
        </q-card>
      </q-tab-panel>

      <!-- apenas rodas -->
      <q-tab-panel name="rodas" class="flex flex-center q-gutter-sm">
       <!-- card Encoders -->
        <q-card class="flex flex-center q-pa-sm cardPadrao">
          <q-card-section class="flex flex-center q-pa-none">
            <div>Roda Direita</div>
          </q-card-section>

          <q-separator class="full-width" blue inset />

          <q-card-section class="flex flex-center q-pa-none q-gutter-x-md">
            <q-knob disable
              show-value
              class="text-light-green-6 q-my-sm"
              v-model="Di"
              size="4rem"
              color="light-green-6"
            >
              <div>Di : {{Di}}</div>
            </q-knob>
          </q-card-section>
        </q-card>

        <q-card class="flex flex-center q-pa-sm cardPadrao">
          <q-card-section class="full-width flex flex-center q-pa-none">
            <div>Roda Esquerda</div>
          </q-card-section>

          <q-separator class="full-width" blue inset />

          <q-card-section class="flex flex-center q-pa-none q-gutter-x-md">
            <q-knob disable
              show-value
              class="text-lime-6 q-my-sm"
              v-model="Es"
              size="4rem"
              color="lime-6"
            >
              <div>Es : {{Es}}</div>
            </q-knob>
          </q-card-section>
        </q-card>
      </q-tab-panel>

    </q-tab-panels>

    <q-btn class="full-width" color="red" @click="desconnect()" label="desconectar" />
  </div>
</template>

<style scoped lang="scss">
  .cardPadrao {
    min-width: 175px;
  }
</style>

<script>
import { defineComponent,ref } from 'vue'
import { useQuasar } from 'quasar';

export default defineComponent({
  preFetch ({ redirect }) {
  },
  setup () {
    const $q = useQuasar()
    return {
      tab: ref('all'),
      velocidade: ref(80),
      rpm: ref(50),
      Di: ref(20),
      Es: ref(20),
      options: [],

      desconnect(){
        $q.notify({
          progress: false,
          message: 'Disconnected!',
          icon: 'bluetooth',
          color: 'red',
          textColor: 'white',
          timeout: 1000
        })
        this.$router.push({name: "login"})
      }
    }
  }
})
</script>
