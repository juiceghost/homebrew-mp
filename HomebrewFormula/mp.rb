class Mp < Formula
  desc "Console-based editor for programmers."
  homepage "https://github.com/juiceghost/homebrew-mp
  url "https://github.com/juiceghost/homebrew-mp/archive/v3.2.14a.tar.gz"
  sha256 "b4f0bde929526999eca8d161b5c345003b1c383104142e2f6401ab67c83d4371"

  def install
    system "make", "mp"
    bin.install "mp"
  end

  test do
    assert_match "3.2.13", shell_output("#{bin}/mp --version")
  end
end
