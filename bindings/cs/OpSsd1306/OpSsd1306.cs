using System.Buffers;
using System.Runtime.InteropServices;

namespace Sorac.OpSsd1306;

internal static partial class Native
{
    private const string LibraryName = "opssd1306";

    [LibraryImport(LibraryName, EntryPoint = nameof(ssd1306_open))]
    public static partial nint ssd1306_open(int sda, int scl, int line_height);

    [LibraryImport(LibraryName, EntryPoint = nameof(ssd1306_close))]
    public static partial void ssd1306_close(nint handle);

    [LibraryImport(LibraryName, EntryPoint = nameof(ssd1306_set_pixel))]
    public static partial void ssd1306_set_pixel(nint handle, int row, int col, byte value);

    [LibraryImport(LibraryName, EntryPoint = nameof(ssd1306_set_page_col))]
    public static partial void ssd1306_set_page_col(nint handle, int page, int col);

    [LibraryImport(LibraryName, EntryPoint = nameof(ssd1306_clear))]
    public static partial void ssd1306_clear(nint handle);

    [LibraryImport(LibraryName, EntryPoint = nameof(ssd1306_syncln))]
    public static partial void ssd1306_syncln(nint handle);

    [LibraryImport(LibraryName, EntryPoint = nameof(ssd1306_scrl_down))]
    public static partial void ssd1306_scrl_down(nint handle);

    [LibraryImport(LibraryName, EntryPoint = nameof(ssd1306_putchar))]
    public static partial void ssd1306_putchar(nint handle, nint font, uint c);

    [LibraryImport(LibraryName, EntryPoint = nameof(ssd1306_print))]
    public static partial void ssd1306_print(nint handle, nint font, [In] uint[] str);

    [LibraryImport(LibraryName, EntryPoint = nameof(font_load), StringMarshalling = StringMarshalling.Utf8)]
    public static partial nint font_load(string path);

    [LibraryImport(LibraryName, EntryPoint = nameof(font_free))]
    public static partial void font_free(nint font);

    static Native() {
        if (!RuntimeInformation.IsOSPlatform(OSPlatform.Linux))
            throw new PlatformNotSupportedException("This library is only supported on linux-arm64.");

        if (!RuntimeInformation.OSArchitecture.Equals(Architecture.Arm64))
            throw new PlatformNotSupportedException("This library is only supported on linux-arm64.");
    }
}

public class Font(string path) : IDisposable
{
    private readonly nint _handle = Native.font_load(path);
    internal nint Handle =>
        _disposed ? throw new ObjectDisposedException(nameof(Font)) : _handle;

    #region IDisposable

    private bool _disposed;

    private void Dispose(bool disposing)
    {
        if (_disposed) return;

        if (disposing) { }

        Native.font_free(_handle);
        _disposed = true;
    }

    public void Dispose()
    {
        Dispose(true);
        GC.SuppressFinalize(this);
    }

    ~Font() { Dispose(false); }

    #endregion
}

public class OpSsd1306(int sdaPort, int sclPort, int lineHeight) : IDisposable
{
    private readonly nint _handle = Native.ssd1306_open(sdaPort, sclPort, lineHeight);
    internal nint Handle =>
        _disposed ? throw new ObjectDisposedException(nameof(OpSsd1306)) : _handle;

    public void SetPixel(int row, int column, byte value) =>
        Native.ssd1306_set_pixel(Handle, row, column, value);

    public void SetPageColumn(int page, int column) =>
        Native.ssd1306_set_page_col(Handle, page, column);

    public void Clear() => Native.ssd1306_clear(Handle);

    public void SyncLine() => Native.ssd1306_syncln(Handle);

    public void ScrollDown() => Native.ssd1306_scrl_down(Handle);

    public void PutChar(Font font, char c) => Native.ssd1306_putchar(Handle, font.Handle, c);

    public void Print(Font font, string str)
    {
        var expanded = ArrayPool<uint>.Shared.Rent(str.Length + 1);
        try
        {
            for (var i = 0; i < str.Length; i++)
                expanded[i] = str[i];
            expanded[str.Length] = 0;
            Native.ssd1306_print(Handle, font.Handle, expanded);
        }
        finally
        {
            ArrayPool<uint>.Shared.Return(expanded);
        }
    }

    #region IDisposable

    private bool _disposed;

    private void Dispose(bool disposing)
    {
        if (_disposed) return;

        if (disposing) { }

        Native.ssd1306_close(_handle);
        _disposed = true;
    }

    public void Dispose()
    {
        Dispose(true);
        GC.SuppressFinalize(this);
    }

    ~OpSsd1306() { Dispose(false); }

    #endregion

}
