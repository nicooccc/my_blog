<!-- synced-from-vault: 02-计算机基础/计算机组成原理/CSAPP_二进制炸弹实验题解报告.md | updated: - -->
# CS:APP 二进制炸弹（Binary Bomb Lab）完整题解与逆向实验报告

---

## 1. 实验概况与答案速查

### 1.1 实验背景
本实验基于经典计算机系统基础实验（CS:APP Binary Bomb Lab），实验目标是通过阅读 x86 汇编代码，结合 GDB 动态调试工具，分析控制流、循环、分支、递归、数组、结构体与指针等底层机制，找出使得炸弹不发生爆炸（避免调用 `explode_bomb`）的解除密码。

### 1.2 炸弹通关答案速查表

| 关卡 (Phase) | 通关输入 (Answer) | 核心考察机制 |
| :--- | :--- | :--- |
| **Phase 1** | `The moon unit will be divided into two divisions.` | 内存字符串比对 (`strings_not_equal`) |
| **Phase 2** | `0 1 3 6 10 15` *(或 `1 2 4 7 11 16`)* | 循环与数列递推 ($a_0 \ge 0, a_i = a_{i-1} + i$) |
| **Phase 3** | `4 0` *(或 `0 -396`, `1 -561` 等)* | `switch-case` 跳转表与穿透执行 (Fallthrough) |
| **Phase 4** | `6 6` *(彩蛋输入: `6 6 DrEvil`)* | 二分搜索递归树 (`func4` 要求返回值 6) |
| **Phase 5** | `jdoefg` *(或 `JDOEFG`)* | 字符低 4 位查表映射成 `"oilers"` |
| **Phase 6** | `4 1 6 3 5 2` | 链表节点重排（按节点数值降序排列） |
| **Secret Phase** | `1001` | 二叉搜索树右倾遍历 (`fun7` 返回值 7) |

---

## 2. Phase 1：字符串匹配

### 2.1 汇编代码分析
```assembly
08048bce <phase_1>:
 8048bce: push   %ebp
 8048bcf: mov    %esp,%ebp
 8048bd1: sub    $0x18,%esp
 8048bd4: movl   $0x804a374,0x4(%esp)    ; [参数2] 预设标准字符串地址 0x804a374
 8048bdc: mov    0x8(%ebp),%eax          ; 取出函数入参（用户输入的字符串指针）
 8048bdf: mov    %eax,(%esp)             ; [参数1] 用户输入的字符串
 8048be2: call   8049178 <strings_not_equal> ; 调用字符串比较函数
 8048be7: test   %eax,%eax               ; 测试返回值是否为 0
 8048be9: je     8048bf0 <phase_1+0x22>  ; 若 %eax == 0 (ZF=1)，跳过爆炸指令
 8048beb: call   80493db <explode_bomb>  ; 触发爆炸！
 8048bf0: leave
 8048bf1: ret
```

### 2.2 逆向推导与原理解析
1. `strings_not_equal` 接收两个字符串参数：用户输入字符串和地址 `0x804a374` 处的字符串。
2. 当两个字符串完全一致时，`strings_not_equal` 返回 `0`；否则返回非 0。
3. `test %eax, %eax` 运算后，若 `%eax == 0`，零标志位 $ZF = 1$，`je` 触发跳转，跳过 `explode_bomb`。

### 2.3 GDB 调试命令
```gdb
(gdb) b phase_1
(gdb) r
(gdb) x/s 0x804a374
0x804a374: "The moon unit will be divided into two divisions."
```

**本关答案**：
```text
The moon unit will be divided into two divisions.
```

---

## 3. Phase 2：循环与数列递推

### 3.1 汇编代码分析
```assembly
08048bf2 <phase_2>:
 8048bf2: push   %ebp
 8048bf3: mov    %esp,%ebp
 8048bf5: sub    $0x38,%esp
 8048bf8: lea    -0x24(%ebp),%eax        ; 栈上数组首地址 a[0]
 8048bfb: mov    %eax,0x4(%esp)          ; 参数2: 存放6个整数的数组缓冲区
 8048bff: mov    0x8(%ebp),%eax          ; 用户输入字符串
 8048c02: mov    %eax,(%esp)             ; 参数1
 8048c05: call   80490e3 <read_six_numbers> ; 读取 6 个整数
 8048c0a: mov    -0x24(%ebp),%eax        ; 取出 a[0]
 8048c0d: test   %eax,%eax
 8048c0f: jns    8048c16 <phase_2+0x24>  ; 要求 a[0] >= 0 (符号位为 0)
 8048c11: call   80493db <explode_bomb>
 8048c16: movl   $0x1,-0xc(%ebp)         ; 循环变量 i = 1 (-0xc(%ebp))
 8048c1d: jmp    8048c42 <phase_2+0x50>
; 循环体 (i = 1 到 5):
 8048c1f: mov    -0xc(%ebp),%eax         ; %eax = i
 8048c22: mov    -0x24(%ebp,%eax,4),%eax ; %eax = a[i]
 8048c26: mov    -0xc(%ebp),%edx         ; %edx = i
 8048c29: sub    $0x1,%edx               ; %edx = i - 1
 8048c2c: mov    -0x24(%ebp,%edx,4),%ecx ; %ecx = a[i - 1]
 8048c30: mov    -0xc(%ebp),%edx         ; %edx = i
 8048c33: add    %ecx,%edx               ; %edx = a[i - 1] + i
 8048c35: cmp    %edx,%eax               ; 比较 a[i] 与 a[i - 1] + i
 8048c37: je     8048c3e <phase_2+0x4c>  ; 相等则继续循环
 8048c39: call   80493db <explode_bomb>
 8048c3e: addl   $0x1,-0xc(%ebp)         ; i++
 8048c42: cmpl   $0x5,-0xc(%ebp)         ; i <= 5 ?
 8048c46: jle    8048c1f <phase_2+0x2d>
 8048c48: leave
 8048c49: ret
```

### 3.2 逆向推导与原理解析
1. `read_six_numbers` 使用 `"%d %d %d %d %d %d"` 格式将输入解析为 6 个整数，存入数组 $a[0 \dots 5]$。
2. 基础检查：$a[0] \ge 0$。
3. 循环递推公式：
   $$a[i] = a[i - 1] + i \quad (1 \le i \le 5)$$
4. 取 $a[0] = 0$ 进行计算：
   * $a[1] = 0 + 1 = 1$
   * $a[2] = 1 + 2 = 3$
   * $a[3] = 3 + 3 = 6$
   * $a[4] = 6 + 4 = 10$
   * $a[5] = 10 + 5 = 15$

**本关答案**：
```text
0 1 3 6 10 15
```

---

## 4. Phase 3：跳转表与 Switch 穿透

### 4.1 汇编代码分析
```assembly
08048c4a <phase_3>:
 8048c50: movl   $0x0,-0xc(%ebp)         ; sum = 0 (累加器)
 8048c5e: lea    -0x18(%ebp),%eax        ; &num2
 8048c61: mov    %eax,0xc(%esp)          ; 参数4
 8048c65: lea    -0x14(%ebp),%eax        ; &num1
 8048c68: mov    %eax,0x8(%esp)          ; 参数3
 8048c6c: movl   $0x804a3a6,0x4(%esp)    ; 参数2: 格式化字符串 ("%d %d")
 8048c74: mov    0x8(%ebp),%eax          ; 参数1: 输入行
 8048c7a: call   8048890 <__isoc99_sscanf@plt>
 8048c82: cmpl   $0x1,-0x10(%ebp)        ; sscanf 返回值 > 1 ? (至少读入2个数)
 8048c86: jg     8048c8d <phase_3+0x43>
 8048c88: call   80493db <explode_bomb>
 8048c8d: mov    -0x14(%ebp),%eax        ; %eax = num1
 8048c90: cmp    $0x7,%eax
 8048c93: ja     8048cd5 <phase_3+0x8b>  ; 无符号比较: num1 > 7 则爆炸
 8048c95: mov    0x804a3ac(,%eax,4),%eax ; 取跳转表基址 0x804a3ac 处的表项
 8048c9c: jmp    *%eax                   ; 间接跳转到对应分支！
; 分支体 (注意：没有 break，向下持续穿透 Fallthrough):
 8048c9e: addl   $0xa5,-0xc(%ebp)        ; case 0: sum += 165
 8048ca5: subl   $0x9d,-0xc(%ebp)        ; case 1: sum -= 157
 8048cac: addl   $0x37,-0xc(%ebp)        ; case 2: sum += 55
 8048cb0: subl   $0x1cb,-0xc(%ebp)       ; case 3: sum -= 459
 8048cb7: addl   $0x1cb,-0xc(%ebp)       ; case 4: sum += 459
 8048cbe: subl   $0x1cb,-0xc(%ebp)       ; case 5: sum -= 459
 8048cc5: addl   $0x1cb,-0xc(%ebp)       ; case 6: sum += 459
 8048ccc: subl   $0x1cb,-0xc(%ebp)       ; case 7: sum -= 459
 8048cd3: jmp    8048cda <phase_3+0x90>
; 最终校验:
 8048cda: mov    -0x14(%ebp),%eax        ; %eax = num1
 8048cdd: cmp    $0x5,%eax
 8048ce0: jg     8048cea <phase_3+0xa0>  ; 要求 num1 <= 5
 8048ce2: mov    -0x18(%ebp),%eax        ; %eax = num2
 8048ce5: cmp    %eax,-0xc(%ebp)         ; 比较 num2 与 sum
 8048ce8: je     8048cef <phase_3+0xa5>  ; 相等则通过！
 8048cea: call   80493db <explode_bomb>
```

### 4.2 逆向推导与原理解析
1. 输入格式为两个整数 `num1 num2`。
2. 约束条件：$0 \le num1 \le 5$。
3. `switch(num1)` 分支中没有 `break` 语句，从 `case num1` 选定入口进入后，会一直连续向下执行加减运算，直至 `case 7` 结束。
4. 目标：使得累加器中的结果 `sum` 与 `num2` 相等。
5. 最佳选择 $num1 = 4$：
   从 `case 4` 开始计算：
   $$\text{sum} = (+459) + (-459) + (+459) + (-459) = 0$$
   因此对应的 $num2 = 0$。

**本关答案**：
```text
4 0
```

---

## 5. Phase 4：二分搜索与递归

### 5.1 汇编代码分析
```assembly
08048d6e <phase_4>:
 8048d82: movl   $0x804a3a6,0x4(%esp)    ; 格式化字符串 "%d %d"
 8048d90: call   8048890 <__isoc99_sscanf@plt>
 8048d98: cmpl   $0x2,-0xc(%ebp)         ; 必须读入 2 个整数
 8048d9c: jne    8048dad
 8048d9e: mov    -0x18(%ebp),%eax        ; num1
 8048da1: test   %eax,%eax
 8048da3: js     8048dad                 ; num1 < 0 爆炸
 8048da8: cmp    $0xe,%eax               ; 比较 num1 与 14 (0xe)
 8048dab: jle    8048db2                 ; 要求 0 <= num1 <= 14
 8048db2: movl   $0x6,-0x10(%ebp)        ; 设定 target = 6
 8048dbc: movl   $0xe,0x8(%esp)          ; 参数3: high = 14
 8048dc4: movl   $0x0,0x4(%esp)          ; 参数2: low = 0
 8048dcc: mov    %eax,(%esp)             ; 参数1: val = num1
 8048dcf: call   8048cf1 <func4>         ; 调用 func4(num1, 0, 14)
 8048dda: cmp    -0x10(%ebp),%eax        ; 检查 func4 返回值是否等于 6
 8048ddd: jne    8048de7
 8048ddf: mov    -0x1c(%ebp),%eax        ; 取出 num2
 8048de2: cmp    -0x10(%ebp),%eax        ; 检查 num2 是否等于 6
 8048de5: je     8048dec                 ; 均满足则过关！
```

### 5.2 递归函数 `func4(val, low, high)` 分析
`func4` 汇编逻辑还原为数学模型：
$$\text{mid} = \text{low} + \frac{\text{high} - \text{low}}{2}$$
* 若 $val < mid$：向左半区搜索，返回 $2 \times \text{func4}(val, low, mid - 1)$
* 若 $val == mid$：命中中点，返回 $0$
* 若 $val > mid$：向右半区搜索，返回 $2 \times \text{func4}(val, mid + 1, high) + 1$

### 5.3 逆推求值
目标返回值是 `6`（二进制 $110_2$）：
1. 最终返回值为偶数 $6 \implies$ 最后一步为向左递归，且子结果为 $3$；
2. 值为奇数 $3 \implies$ 步骤为向右递归，且子结果为 $(3-1)/2 = 1$；
3. 值为奇数 $1 \implies$ 步骤为向右递归，且子结果为 $(1-1)/2 = 0$；
4. 值为 $0 \implies$ 命中基准情形 $val == mid$。

正向区间跟踪：
* **第 1 层**：$[0, 14], mid = 7 \implies val < 7$（向左走，新区间 $[0, 6]$）
* **第 2 层**：$[0, 6], mid = 3 \implies val > 3$（向右走，新区间 $[4, 6]$）
* **第 3 层**：$[4, 6], mid = 5 \implies val > 5$（向右走，新区间 $[6, 6]$）
* **第 4 层**：$[6, 6], mid = 6 \implies val == 6$（命中，返回 0）

故 $num1 = 6$，且 $num2 = 6$。

> **提示**
> **彩蛋关卡解锁入口**：
> 如果输入 `6 6 DrEvil`，第三个单词 `"DrEvil"` 将在全部 6 关通关后触发隐藏关（Secret Phase）。

**本关答案**：
```text
6 6
```

---

## 6. Phase 5：低 4 位查表解密

### 6.1 汇编代码分析
```assembly
08048dee <phase_5>:
 8048dfa: call   804914c <string_length> ; 计算输入字符串长度
 8048e02: cmpl   $0x6,-0x10(%ebp)        ; 长度必须是 6
 8048e06: je     8048e0d
 8048e08: call   80493db <explode_bomb>
; 循环 6 次处理每个字符:
 8048e1e: movzbl (%eax),%eax             ; 读入第 i 个字符
 8048e24: and    $0xf,%eax               ; 取低 4 位: index = c & 0x0F
 8048e27: movzbl 0x804c1a4(%eax),%eax    ; 查表: new_c = array[index]
 8048e36: mov    %al,(%edx)              ; 写入目标字符串缓冲区
; 比较生成结果:
 8048e42: movb   $0x0,-0x11(%ebp)        ; 写入 '\0' 结尾
 8048e46: movl   $0x804a3cc,0x4(%esp)    ; 目标标准串地址 0x804a3cc ("oilers")
 8048e54: call   8049178 <strings_not_equal>
 8048e59: test   %eax,%eax
 8048e5b: je     8048e62                 ; 匹配成功则通过！
```

### 6.2 密码表与 ASCII 反推
在数据段 `0x804c1a4` 处的 16 字节字符表如下：
| 索引 (Hex) | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A | B | C | D | E | F |
| :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **字符** | `m` | `a` | `d` | `u` | `i` | `e` | `r` | `s` | `n` | `f` | `o` | `t` | `v` | `b` | `y` | `l` |

目标字符串为 `"oilers"`，逐字符反查目标索引：
* `'o'` $\implies$ 索引 `10` (`0x0A`)
* `'i'` $\implies$ 索引 `4`  (`0x04`)
* `'l'` $\implies$ 索引 `15` (`0x0F`)
* `'e'` $\implies$ 索引 `5`  (`0x05`)
* `'r'` $\implies$ 索引 `6`  (`0x06`)
* `'s'` $\implies$ 索引 `7`  (`0x07`)

构造满足低 4 位等于上述索引的小写字母（小写字母高 4 位为 `0x60`）：
* `0x60 | 0x0A = 0x6A` $\implies$ `'j'`
* `0x60 | 0x04 = 0x64` $\implies$ `'d'`
* `0x60 | 0x0F = 0x6F` $\implies$ `'o'`
* `0x60 | 0x05 = 0x65` $\implies$ `'e'`
* `0x60 | 0x06 = 0x66` $\implies$ `'f'`
* `0x60 | 0x07 = 0x67` $\implies$ `'g'`

**本关答案**：
```text
jdoefg
```

---

## 7. Phase 6：链表指针降序排列

### 7.1 汇编代码分析
```assembly
08048e64 <phase_6>:
 8048e7e: call   80490e3 <read_six_numbers> ; 读入 6 个整数
; 校验 1: 每个数字必须属于 [1, 6]，且 6 个数字互不重复
 8048e95: jle    8048ea3                 ; 若 a[i] <= 0 爆炸
 8048ea1: jle    8048ea8                 ; 若 a[i] <= 6 继续，否则爆炸
; 两重循环验证唯一性: a[i] != a[j]

; 节点指针重排:
 8048e6a: movl   $0x804c0e4,-0x18(%ebp)  ; 链表头节点 node1 地址
; 按照输入的 6 个数字作为序号，把对应的节点指针存入指针数组 arr[0..5]
; 重新拼接链表指针: arr[0]->next = arr[1], arr[1]->next = arr[2] ... arr[5]->next = NULL

; 校验 2: 遍历重构后的链表，检查节点数值是否递减
 8048f74: mov    (%eax),%edx             ; cur->val
 8048f79: mov    0x8(%eax),%eax          ; cur->next
 8048f7c: mov    (%eax),%eax             ; cur->next->val
 8048f7e: cmp    %eax,%edx               ; 比较 cur->val 与 cur->next->val
 8048f80: jge    8048f87                 ; 要求 cur->val >= cur->next->val (从大到小排列)
 8048f82: call   80493db <explode_bomb>  ; 否则爆炸
```

### 7.2 节点结构体与数值提取
数据段中每个节点占用 12 字节：`[4字节 数值] [4字节 序号] [4字节 下一节点指针]`。
从内存查看各节点数据：
* **Node 1** (`0x804c0e4`): 值 = `0x02db` = **731**
* **Node 2** (`0x804c0d8`): 值 = `0x008f` = **143**
* **Node 3** (`0x804c0cc`): 值 = `0x0198` = **408**
* **Node 4** (`0x804c0c0`): 值 = `0x03c0` = **960**
* **Node 5** (`0x804c0b4`): 值 = `0x00e4` = **228**
* **Node 6** (`0x804c0a8`): 值 = `0x026c` = **620**

### 7.3 排序与推导
将节点按数值降序（从大到小）排序：
$$960(\text{Node 4}) > 731(\text{Node 1}) > 620(\text{Node 6}) > 408(\text{Node 3}) > 228(\text{Node 5}) > 143(\text{Node 2})$$

排序后的序号序列即为输入答案：`4 1 6 3 5 2`。

**本关答案**：
```text
4 1 6 3 5 2
```

---

## 8. 彩蛋关（Secret Phase）：二叉搜索树

### 8.1 触发条件
在 `phase_defused` 函数中检测到 Phase 4 所在行的输入内容，使用格式化串 `"%d %d %s"` 解析：
当成功解析出 3 项且第三项字符串等于 `"DrEvil"` 时，通关 Phase 6 后便会进入 `secret_phase`。
在 Phase 4 输入：
```text
6 6 DrEvil
```

### 8.2 汇编代码与二叉树逻辑
`secret_phase` 读入一个整数 $val$，并调用 `fun7(root, val)`，要求返回值必须为 `7`：
```assembly
08048f9c <fun7>:
  if (root == NULL) return -1;
  if (root->val > val) {
      return 2 * fun7(root->left, val);
  } else if (root->val == val) {
      return 0;
  } else {
      return 2 * fun7(root->right, val) + 1;
  }
```

### 8.3 逆推路径
返回值需要为 `7`：
* $7 = 2 \times 3 + 1 \implies$ 第 1 步向**右**走，子树需返回 3；
* $3 = 2 \times 1 + 1 \implies$ 第 2 步向**右**走，子树需返回 1；
* $1 = 2 \times 0 + 1 \implies$ 第 3 步向**右**走，子树需返回 0；
* 最终命中 $val == \text{node.val}$。

查看根节点 `n1` (`0x804c198`) 出发的二叉树右子树链：
1. 根节点 `n1`: 值 36 $\to$ 向右走至 `n22`；
2. 节点 `n22`: 值 50 $\to$ 向右走至 `n34`；
3. 节点 `n34`: 值 107 $\to$ 向右走至 `n48`；
4. 节点 `n48` (`0x804c0f0`): 其存放的数值为 `0x03e9` = **1001**。

因此 $val = 1001$。

**隐藏关答案**：
```text
1001
```

---

## 9. GDB 调试高频指令速查表

| 指令 | 作用说明 | 示例 |
| :--- | :--- | :--- |
| `b <func>` | 在指定函数入口设置断点 | `b phase_1` |
| `b *<addr>` | 在指定汇编指令物理地址设置断点 | `b *0x8048be2` |
| `r` | 开始运行程序 | `r` |
| `c` | 继续执行至下一个断点 | `c` |
| `ni` / `si` | 单步步过 / 单步步入一条汇编指令 | `ni` |
| `x/s <addr>` | 以 C 字符串格式查看内存地址 | `x/s 0x804a374` |
| `x/dw <addr>` | 以十进制整数查看指定内存 | `x/d $ebp-0xc` |
| `x/xw <addr>` | 以十六进制查看指定内存 | `x/x 0x804c0e4` |
| `x/6dw <addr>` | 查看连续 6 个十进制整数 | `x/6dw $esp+0x10` |
| `info registers` | 查看所有寄存器的当前值 | `i r eax` |

<!-- vault-sync: 由 D:/library/nico 自动同步，请勿在博客侧手工编辑 -->
