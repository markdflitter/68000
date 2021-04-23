.global __main
.global start
start:
			MOVE.B #3,0xE00001
			MOVEA.L #0x2FFFFC,%SP
			MOVE.B #4,0xE00001

			JSR __main

			MOVE.B #9,0xE00001

			MOVE.L 4,%A0
			JMP (%A0)
