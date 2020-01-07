class Mp < Formula
  desc "Minimum Profit editor by Angel Ortega"
  homepage "https://triptico.com/software/mp.html"
  url "https://github.com/juiceghost/homebrew-mp"

  def install
    system "make"
    bin.install "mp"
  end
  
  test do
    assert_match "3.2.13", shell_output("#{bin}/mp --version", 2)
  end
end