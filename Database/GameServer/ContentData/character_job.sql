DROP TABLE IF EXISTS `job`;
CREATE TABLE  `job` (
  `CharID` integer NOT NULL,
  `alias` varchar(20) NOT NULL,
  `type` int(1) NOT NULL,
  `level` int(1) NOT NULL,
  `exp` int(11) NOT NULL
) ENGINE=MyISAM DEFAULT CHARSET=utf8;
