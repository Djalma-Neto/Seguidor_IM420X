<template>
  <q-page class="flex flex-center">
    <img
      alt="Conectando BlueTooth"
      src="~assets/connectingBlueTooth.gif"
      style="height:20%; opacity:0.9;"
      class="absolute-center"
      @click="startConnect()"
    />
  </q-page>
</template>

<script>
import { defineComponent,ref } from "vue";
import { useQuasar } from 'quasar';

export default defineComponent({
  name: "PageIndex",

  preFetch ({ redirect }) {
  },
  setup () {
    const $q = useQuasar()
    return {
      orientationClass: $q.screen.height,

      startConnect(){
        console.log('conectando!...')
        setTimeout(() => {
          $q.notify({
            progress: false,
            message: 'Connected!',
            icon: 'bluetooth',
            color: 'green',
            textColor: 'white',
            timeout: 1000
          })
          this.$router.push({name: "dashBoard"})
        }, 2000)
      }
    }
  }
});
</script>
