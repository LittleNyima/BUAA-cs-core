.data
symbol: .space 32
array: .space 32
space: .asciiz " "
enter: .asciiz "\n"

.text
# $s0 for n, $s7 for const 1
li $s7, 1
li $v0, 5
syscall
move $s0, $v0

# $s1 for index, $s2 for i
li $s1, 0
li $s2, 0
addi $sp, $sp, -4
sw $s1, 0($sp)
addi $sp, $sp, -4
sw $s2, 0($sp)
jal fullarray

li $v0, 10
syscall
fullarray:
addi $sp, $sp, -4
sw $ra, 0($sp)
if_1:
slt $t0, $s1, $s0
bne $t0, $0, end_if_1

li $s2, 0
for_1:
slt $t0, $s2, $s0
beq $t0, $0, end_for_1

sll $t2, $s2, 2
lw $a0, array($t2)
li $v0, 1
syscall

la $a0, space
li $v0, 4
syscall

addi $s2, $s2, 1
j for_1
end_for_1:
la $a0, enter
li $v0, 4
syscall

end_if_1:

li $s2, 0
for_2:
slt $t0, $s2, $s0
beq $t0, $0, end_for_2

if_2:
sll $t3, $s2, 2
lw $t1, symbol($t3)
seq $t0, $t1, $0
beq $t0, $0, end_if_2

addi $t1, $s2, 1
sll $t3, $s1, 2
sw $t1, array($t3)
sll $t3, $s2, 2
sw $s7, symbol($t3)
addi $sp, $sp, -4
sw $s1, 0($sp)
addi $sp, $sp, -4
sw $s2, 0($sp)
addi $s1, $s1, 1
jal fullarray
sll $t3, $s2, 2
sw $0, symbol($t3)
end_if_2:

addi $s2, $s2, 1
j for_2

end_for_2:

lw $ra, 0($sp)
addi $sp, $sp, 4
lw $s2, 0($sp)
addi $sp, $sp, 4
lw $s1, 0($sp)
addi $sp, $sp, 4
jr $ra

li $v0, 10
syscall