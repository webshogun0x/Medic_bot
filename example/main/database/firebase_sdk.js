// Import the functions you need from the SDKs you need
import { initializeApp } from "firebase/app";
// TODO: Add SDKs for Firebase products that you want to use
// https://firebase.google.com/docs/web/setup#available-libraries

// Your web app's Firebase configuration
const firebaseConfig = {
  apiKey: "AIzaSyCCuZcfDnHjxi-oCeTin3QjHZPMXwI_nF8",
  authDomain: "medic-bot-health-monitor.firebaseapp.com",
  databaseURL: "https://medic-bot-health-monitor-default-rtdb.firebaseio.com",
  projectId: "medic-bot-health-monitor",
  storageBucket: "medic-bot-health-monitor.firebasestorage.app",
  messagingSenderId: "788008894165",
  appId: "1:788008894165:web:20ac5d45f1b1a65828ac5c"
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);