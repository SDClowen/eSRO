DROP TABLE IF EXISTS `activation`;

CREATE TABLE `activation` (
  `activationID` int(25) NOT NULL AUTO_INCREMENT,
  `activationLink` varchar(255) DEFAULT NULL,
  `Username` varchar(255) DEFAULT NULL,
  `Password` varchar(255) DEFAULT NULL,
  `Email` varchar(255) DEFAULT NULL,
  PRIMARY KEY (`activationID`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
