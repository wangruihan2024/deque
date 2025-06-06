# Deque README


## 项目概述

在小作业中，大家已经使用过STL的 `std::deque` 的头尾插入删除功能，实际上，`std::deque` 也支持类似普通数组那样的随机下标访问。


<img src="https://www.oreilly.com/api/v2/epubs/9781787120952/files/assets/fd7f0c6e-e5cb-400d-ad2f-c38e91772682.png" width="500">

在这次大作业中，你需要使用分块链表（Unrolled Linked List）数据结构实现高效的双端队列（Deque），支持快速随机访问与动态容量调整。所需要实现的接口在给定的文件中。

## 核心要求

分块链表就是对存储元素进行分块，以加速随机访问等操作。理想情况下，每一块的大小在 $O(\sqrt n)$ 量级。一种保证方法是在块过大时分成两块，相邻两块均很小时合成一块。在具体实现中，每一
块内部的储存方式和所有块的储存方式均可使用链表或数组。 你需要保证头尾插入和删除的均摊
复杂度是 $O(1)$，随机插入、删除和查询的复杂度是最坏 $O(\sqrt n)$。

这次的大作业不允许使用STL容器，但你可以使用你上次写的双向链表，直接粘贴进 `src.hpp` 即可。这次任务的几乎所有测试都在下发的文件中，其中带有 memcheck 的测试仅是其原始测试数据更少
的版本，可用作检查内存泄漏和内存错误。

我们在此提供两种可以考虑的实现方式：一种是链表套链表，另一种的链表套循环数组，一般来说后一种实现的效率会更高一些。**注：你的实现必须包括动态调整块长的策略，不允许固定块长**

## 项目结构

```
.
├── tests/
│   ├── one/             # 功能测试用例
│   ├── two/      
│   ├── three/      
│   ├── four/      
│   ├── two.memcheck/    # 内存检查专用测试
│   └── four.memcheck/    
├── (various utility hpp files...)
├── README.md            # 你给deque写的文档
└── deque.hpp            # 你的deque实现
```

## 对于 `README.md` 的要求

你需要在 `README.md` 中给出自己分裂和合并的策略，并说明为什么时间复杂度符合要求。

**Note: 如果说明非常清晰，会有额外1-5分加分**

另附：最好认真写，因为我们 CR 的时候一定会问

## 时间安排

这次大作业的发布时间为2025年3月10日，截止时间是2025年4月7日

## 策略说明

### 实现方法

双层链表实现

### 分裂合并策略

若当前块长大于$\sqrt{n} * 分裂系数$时，会分成两个原块长$1/2$的块

若当前块长小于$\sqrt{n} * 合并系数$时，会将当前快与左右两个中块长较小的合并

注：若当前快在队首则与后一块合并， 若在块尾则与前一块合并

### 分裂合并时间复杂度

单次合并时间复杂度: $O(1)$

单次分类时间复杂度：将原来的块erase, 构造成两个块$O(\sqrt{n})$


### 首位插入删除时间复杂度

策略：插入（删除） + 分裂（合并）

插入删除：等价于链表首位插入删除时间复杂度$O(1)$

均摊分裂：均摊仍然为$O(1)$

均摊合并：若达到合并标准，至少需要$O(\sqrt{n})$次插入（删除）操作，均摊时间复杂度为$O(\sqrt{n}) / O(\sqrt{n}) = O(1)$

均摊时间复杂度：$O(1)$

### 随机插入删除 查询时间复杂度

#### 随机查询

策略：若查询位置小于总长度的$1/2$正着加（返回begin() + pos）否则倒着算

若在一个块长的范围内，即$\sqrt{n}$内，则线性时间 $O(\sqrt{n})$

若大于一个块长, 向前移动$list$， 若移动$n$个，时间$O(n / \sqrt{n}) = O(\sqrt{n})$

复杂度：$O(\sqrt{n})$

#### 随机插入删除

策略：查询 + 插入删除 + 分裂合并

查询：$O(\sqrt{n})$

插入删除：$O(1)$

分裂合并：$O(\sqrt{n})$

复杂度：$O(\sqrt{n})$
