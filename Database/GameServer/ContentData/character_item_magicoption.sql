DROP TABLE IF EXISTS `item_magicoptions`;
CREATE TABLE  `item_magicoptions` (
  `name` varchar(20) NOT NULL,
  `ID` int(11) NOT NULL,
  `Position` int(11) NOT NULL,
  `Place` int(1) NOT NULL,
  `optionID` int(11) NOT NULL,
  `stat` int(11) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
