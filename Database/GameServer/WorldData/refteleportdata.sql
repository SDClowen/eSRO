DROP TABLE IF EXISTS `refteleportdata`;
CREATE TABLE  `refteleportdata` (
  `ID` int(11) NOT NULL,
  `Name` varchar(30) NOT NULL,
  `AssignedID` int(11) NOT NULL,
  `Zone` int(11) NOT NULL,
  `X` float NOT NULL,
  `Z` float NOT NULL,
  `Y` float NOT NULL,
  UNIQUE KEY `ID` (`ID`)
);
