import React from 'react'

export type ThemeType = 'light' | 'dark'

export interface ISiteContext {
  theme: ThemeType
}

export const SiteContext = React.createContext<ISiteContext>({
  theme: 'dark'
})
