# constant 1 $s0
# $s1, $s2 for n, m
# current point maze[$s3][$s4]
# end point maze[$s5][$s6]
# stack offset $s7
# path count $a0
# macro return value $t9
# macro protected reg $t8
# base address $t7

.macro getMatAddr(%x, %y)
mul $t8, $s2, %x
add $t8, $t8, %y
sll $t8, $t8, 2
add $t9, $t7, $t8
.end_macro

.data
maze: .space 256
symbol: .space 256
stackX: .space 256
stackY: .space 256

.text
li $s0, 1
li $a0, 0

# read n, m
li $v0, 5
syscall
move $s1, $v0
li $v0, 5
syscall
move $s2, $v0

# read puzzle
li $t1, 0
read:
slt $t0, $t1, $s1
beq $t0, $0, end_read

li $t2, 0
read_inner:
slt $t0, $t2, $s2
beq $t0, $0, end_read_inner

la $t7, maze
getMatAddr($t1, $t2)

li $v0, 5
syscall
sw $v0, 0($t9)

addi $t2, $t2, 1
j read_inner
end_read_inner:

addi $t1, $t1, 1
j read
end_read:

# read last 4 values
li $v0, 5
syscall
move $s3, $v0
addi $s3, $s3, -1

li $v0, 5
syscall
move $s4, $v0
addi $s4, $s4, -1

li $v0, 5
syscall
move $s5, $v0
addi $s5, $s5, -1

li $v0, 5
syscall
move $s6, $v0
addi $s6, $s6, -1
# end

li $s7, 0
sw $s3, stackX($s7)
sw $s4, stackY($s7)
addi $s7, $s7, 4

la $t7, symbol
getMatAddr($s3, $s4)
sw $s0, 0($t9)

jal search
li $v0, 1
syscall
li $v0, 10
syscall

search:
addi $sp, $sp, -4
sw $ra, 0($sp)

beq $s7, $0, return
lw $s3, stackX+-4($s7)
lw $s4, stackY+-4($s7)

bne $s3, $s5, not_return
bne $s4, $s6, not_return

addi $a0, $a0, 1
j return

not_return:

if_1:
addi $t4, $s4, -1
slt $t0, $0, $s4
beq $t0, $0, end_if_1
la $t7, maze
getMatAddr($s3, $t4)
lw $t1, 0($t9)
bne $t1, $0, end_if_1
la $t7, symbol
getMatAddr($s3, $t4)
lw $t1, 0($t9)
bne $t1, $0, end_if_1

sw $s3, stackX($s7)
sw $t4, stackY($s7)
addi $s7, $s7, 4
la $t7, symbol
getMatAddr($s3, $t4)
sw $s0, 0($t9)

sw $s3, -4($sp)
sw $s4, -8($sp)
addi $sp, $sp, -8

jal search

lw $s4, 0($sp)
lw $s3, 4($sp)
addi $sp, $sp, 8

addi $s7, $s7, -4
addi $t4, $s4, -1
la $t7, symbol
getMatAddr($s3, $t4)
sw $0, 0($t9)

end_if_1:

if_2:
addi $t6, $s2, -1
addi $t4, $s4, 1
slt $t0, $s4, $t6
beq $t0, $0, end_if_2
la $t7, maze
getMatAddr($s3, $t4)
lw $t1, 0($t9)
bne $t1, $0, end_if_2
la $t7, symbol
getMatAddr($s3, $t4)
lw $t1, 0($t9)
bne $t1, $0, end_if_2

sw $s3, stackX($s7)
sw $t4, stackY($s7)
addi $s7, $s7, 4
la $t7, symbol
getMatAddr($s3, $t4)
sw $s0, 0($t9)

sw $s3, -4($sp)
sw $s4, -8($sp)
addi $sp, $sp, -8

jal search

lw $s4, 0($sp)
lw $s3, 4($sp)
addi $sp, $sp, 8

addi $t7, $t7, -4
addi $t4, $s4, 1
la $t7, symbol
getMatAddr($s3, $t4)
sw $0, 0($t9)

end_if_2:

if_3:
addi $t3, $s3, -1
slt $t0, $0, $s3
beq $t0, $0, end_if_3
la $t7, maze
getMatAddr($t3, $s4)
lw $t1, 0($t9)
bne $t1, $0, end_if_3
la $t7, symbol
getMatAddr($t3, $s4)
lw $t1, 0($t9)
bne $t1, $0, end_if_3

sw $t3, stackX($s7)
sw $s4, stackY($s7)
addi $s7, $s7, 4
la $t7, symbol
getMatAddr($t3, $s4)
sw $s0, ($t9)

sw $s3, -4($sp)
sw $s4, -8($sp)
addi $sp, $sp, -8

jal search

lw $s4, 0($sp)
lw $s3, 4($sp)
addi $sp, $sp, 8

addi $s7, $s7, -4
addi $t3, $s3, -1

la $t7, symbol
getMatAddr($t3, $s4)
sw $0, 0($t9)
end_if_3:

if_4:
addi $t3, $s3, 1
addi $t6, $s1, -1
slt $t0, $s3, $t6
beq $t0, $0, end_if_4
la $t7, maze
getMatAddr($t3, $s4)
lw $t1, 0($t9)
bne $t1, $0, end_if_4
la $t7, symbol
getMatAddr($t3, $s4)
lw $t1, 0($t9)
bne $t1, $0, end_if_4

sw $t3, stackX($s7)
sw $s4, stackY($s7)
addi $s7, $s7, 4

la $t7, symbol
getMatAddr($t3, $s4)
sw $s0, 0($t9)

sw $s3, -4($sp)
sw $s4, -8($sp)
addi $sp, $sp, -8

jal search

lw $s4, 0($sp)
lw $s3, 4($sp)
addi $sp, $sp, 8

addi $s7, $s7, -4
addi $t3, $s3, 1
la $t7, symbol
getMatAddr($t3, $s4)
sw $0, 0($t9)
end_if_4:

return:
lw $ra, 0($sp)
addi $sp, $sp, 4

jr $ra