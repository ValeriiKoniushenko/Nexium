import { useState } from 'react'
import './App.css'
import Top from './components/Top'
import Bottom from './components/Bottom'
import RootLayout from './components/RootLayout'

export default function App() {
  return (
    <>
      <Top />
      <RootLayout />
      <Bottom />
    </>
  )
}
