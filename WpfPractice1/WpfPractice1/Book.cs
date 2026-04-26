using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WpfPractice1;

public enum BookCategory
{
    Novel,
    Science,
    History,
    Programming
}
public class Book
{
    public BookCategory Category {  get; set; }
    public bool IsReserved { get; set; }
    public string? Title { get; set; }
    public string? Author { get; set; }
    public string? Isbn { get; set; }
    public int Year { get; set; }

    public Book() { }
}
