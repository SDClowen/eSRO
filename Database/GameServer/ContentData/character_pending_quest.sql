DROP TABLE IF EXISTS `PendingQuests`;
CREATE TABLE  `PendingQuests` (
  `Charname` varchar(20) NOT NULL,
  `ID` int(11) NOT NULL,
  `Name` varchar(40) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
