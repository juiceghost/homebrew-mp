class Mp < Formula
  desc "Console-based editor by Ángel Ortega"
  homepage "https://triptico.com/software/mp.html"
  url "https://github.com/juiceghost/homebrew-mp/archive/v3.2.14.tar.gz"
  sha256 "52883d4dcf69429cc088efc767a9d425af3e739f1e09d54fbee94190f7f80d87"

  def install
    system "make", "mp"
    bin.install "mp"
  end

  test do
    assert_match "3.2.13", shell_output("#{bin}/mp --version")
  end
end
