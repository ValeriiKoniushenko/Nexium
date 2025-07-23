import React, { useState } from 'react';
import { FilterLogsButton } from './Console.styled'

type Props = {
    thisFilter: string;
};

export const FilterLog: React.FC<Props> = ({ thisFilter }) => {
    const [isActive, setIsActive] = useState(true);

    const toggleState = () => {
        setIsActive(prev => !prev);
    };

    return <FilterLogsButton active={isActive} onClick={toggleState}>{thisFilter}</FilterLogsButton>;
}