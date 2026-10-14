import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;
import org.jsoup.select.Elements;

import java.io.File;
import java.io.IOException;
import java.nio.charset.Charset;
import java.nio.file.Files;
import java.nio.file.Paths;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;


public class WfPlayer {
  static String mnem = "";
  static String index = "-1";
  static Map<String, String> codes = new HashMap();
  static SimpleDateFormat hudf = new SimpleDateFormat("dd.MM.yyyy");
  static SimpleDateFormat isof = new SimpleDateFormat("yyyy-MM-dd");

  public static void main(String args[]) {
    // The sandbox this was built/tested in defaults to a non-UTF-8 locale,
    // which garbles accented player names on stdout unless forced here.
    // Doing this explicitly rather than relying on the server's locale.
    try { System.setOut(new java.io.PrintStream(System.out, true, "UTF-8")); } catch (Exception e) { /* ignore */ }
    File input = new File(args[0]);
    mnem = args[1];
    Document doc = null;
    try {
      codes = loadCountryCodes("/var/www/html/country-eng.dat");
    } catch (Exception e) {
      System.err.println("Error: country codes not loaded." + e.getMessage());
    }
    try {
      doc = Jsoup.parse(input, "UTF-8", "https://www.worldfootball.net");
    } catch (IOException e) {
      e.printStackTrace();
    }
    extractBio(doc);
  }

  // Rewritten for the 2026 worldfootball.net redesign. The old sidebar
  // (".sidebar"/".head"/".data") markup is gone. Name now lives in
  // "h2.item-person" (split across ".person-firstname"/".person-lastname"),
  // and the bio facts live in a <dl> inside ".hs-profile--person", one
  // dt/dd pair per fact (dt class names: person-fullname, person-birthday,
  // person_detail-birth_place-*, person_country-name-*, person-height,
  // person-weight, person-position-*, person_detail-preferred_side-*).
  private static void extractBio(Document doc) {
    Element h2 = doc.selectFirst("h2.item-person");
    String name = " ";
    if (h2 != null) {
      Element efn = h2.selectFirst(".person-firstname");
      Element eln = h2.selectFirst(".person-lastname");
      String fn = efn != null ? efn.text().trim() : "";
      String ln = eln != null ? eln.text().trim() : "";
      name = (fn + " " + ln).trim().replaceAll("\\s+", " ");
    }
    if (name.length() == 0) name = " ";

    String full = name;
    String dob = " ";
    String pob = " ";
    String cobRaw = " ";
    String natRaw = " ";

    Element profile = doc.selectFirst(".hs-profile--person");
    Element dl = profile != null ? profile.selectFirst("dl") : null;
    if (dl != null) {
      Elements dts = dl.select("dt");
      Elements dds = dl.select("dd");
      for (int i = 0; i < dts.size() && i < dds.size(); i++) {
        String cls = dts.get(i).className();
        Element dd = dds.get(i);
        if (cls.contains("person-fullname")) {
          String v = dd.text().trim();
          if (v.length() > 0) full = v;
        } else if (cls.contains("person-birthday")) {
          dob = dd.text().split("\\|")[0].trim();
        } else if (cls.contains("birth_place")) {
          String raw = dd.text().trim();
          pob = raw.replaceAll(",$", "").replaceAll(",", " -");
          int ci = raw.lastIndexOf(",");
          cobRaw = ci >= 0 ? raw.substring(ci + 1).trim() : raw;
        } else if (cls.contains("country-name")) {
          Element img = dd.selectFirst("img");
          natRaw = img != null && img.hasAttr("title") ? img.attr("title") : dd.text().trim();
        }
      }
    }

    String isodob = toIso(dob);
    String nat = codes.getOrDefault(natRaw, natRaw);
    String cob = codes.getOrDefault(cobRaw, cobRaw);
    System.out.println(String.format("%s,%s,%s,%s,%s,%s,%s", mnem, name, full, isodob, nat, pob, cob));
  }

  private static String toIso(String dob) {
    if (dob == null) return "0000-00-00";
    try {
      Date d = hudf.parse(dob);
      return isof.format(d);
    } catch (ParseException e) {
      return "0000-00-00";
    }
  }

  private static Map<String, String> loadCountryCodes(String filename) throws IOException {
    Charset charset = Charset.forName("UTF-8");
    List<String> lines = Files.readAllLines(Paths.get(filename), charset);
    Map<String, String> codes = new HashMap();
    for (String s : lines) {
      String[] parts = s.split(",");
      if (parts.length > 1) {
        codes.put(parts[1], parts[0]);
      }
    }
    return codes;
  }
}
