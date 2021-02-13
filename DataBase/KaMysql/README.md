## MYSQL连接池的设计

思路：一个DBManager管理着多个DBPool，这样可以配置在不同的机器上配置多个DBPool，然后由一个DBManager统一管理。DBManager可以使用不用的数据结构对DBPool进行管理，简单的可以使用指针数组，复杂一些的可以使用std::map进行管理。这里采用stl::map对DBPool进行管理。

DBManager采用单例模式：

```C++
class DBManager{
    
private:
	std::map<std::string,DBPool*>		m_dbPools;    
};
```

