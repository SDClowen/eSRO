DROP TABLE IF EXISTS `refshoptab`;
CREATE TABLE `refshoptab` (
	`EntryFlag` INTEGER(1) NOT NULL,
	`ItemFlag` INTEGER(1) NOT NULL,
	`ID` INTEGER NOT NULL,
	`Tab` VARCHAR(50) NOT NULL,
	`GroupTab` VARCHAR(50) NOT NULL,
	`MsgID1` VARCHAR(50) NOT NULL,
	`MsgID2` VARCHAR(4) NOT NULL
);